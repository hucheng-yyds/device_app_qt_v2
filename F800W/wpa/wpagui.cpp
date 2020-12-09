/*
 * wpa_gui - WpaGui class
 * Copyright (c) 2005-2011, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifdef CONFIG_NATIVE_WINDOWS
#include <windows.h>
#endif /* CONFIG_NATIVE_WINDOWS */

#include <cstdio>
#include <unistd.h>
#include <dirent.h>
#include <QMessageBox>
#include <QCloseEvent>
#include <QImageReader>
#include <QSettings>
#include <QDebug>

#include "wpagui.h"
#include "wpa_ctrl.h"
#include "networkconfig.h"

int flagscan=0;
int networkid=0;
int flagadd=0;
int wfauth=6;

WpaGui::WpaGui()
{
	/*
	 * Disable WPS tab by default; it will be enabled if wpa_supplicant is
	 * built with WPS support.
     */
    system("rm /tmp/wpa_ctrl_*");
    tray_icon = nullptr;
	startInTray = false;
	quietMode = false;
    ctrl_iface = nullptr;
    ctrl_conn = nullptr;
    monitor_conn = nullptr;
    msgNotifier = nullptr;
	ctrl_iface_dir = strdup("/tmp/wpa_supplicant");
    ctrl_iface = strdup("wlan0");

	parse_argv();

#ifndef QT_NO_SESSIONMANAGER

#endif
	if (openCtrlConnection(ctrl_iface) < 0) {
		qDebug("Failed to open control connection to "
		      "wpa_supplicant.");
	}

//	updateStatus();
	networkMayHaveChanged = true;
//	updateNetworks();
    removeNetwork();
    currentIconType = TrayIconOffline;
}


WpaGui::~WpaGui()
{
	delete msgNotifier;

	if (monitor_conn) {
		wpa_ctrl_detach(monitor_conn);
		wpa_ctrl_close(monitor_conn);
        printf("close wpagui -------------------------");
        monitor_conn = nullptr;
	}
	if (ctrl_conn) {
		wpa_ctrl_close(ctrl_conn);
        ctrl_conn = nullptr;
	}

	free(ctrl_iface);
    ctrl_iface = nullptr;

	free(ctrl_iface_dir);
    ctrl_iface_dir = nullptr;
}


void WpaGui::languageChange()
{
}


void WpaGui::parse_argv()
{

}


int WpaGui::openCtrlConnection(const char *ifname)
{
	char *cfile;
	int flen;
	char buf[2048], *pos, *pos2;
	size_t len;
	if (ifname) {
		if (ifname != ctrl_iface) {
			free(ctrl_iface);
			ctrl_iface = strdup(ifname);
		}
	} else {
#ifdef CONFIG_CTRL_IFACE_UDP
		free(ctrl_iface);
		ctrl_iface = strdup("udp");
#endif /* CONFIG_CTRL_IFACE_UDP */
#ifdef CONFIG_CTRL_IFACE_UNIX
		struct dirent *dent;
		DIR *dir = opendir(ctrl_iface_dir);
		free(ctrl_iface);
		ctrl_iface = NULL;
		if (dir) {
			while ((dent = readdir(dir))) {
#ifdef _DIRENT_HAVE_D_TYPE
				/* Skip the file if it is not a socket.
				 * Also accept DT_UNKNOWN (0) in case
				 * the C library or underlying file
				 * system does not support d_type. */
				if (dent->d_type != DT_SOCK &&
				    dent->d_type != DT_UNKNOWN)
					continue;
#endif /* _DIRENT_HAVE_D_TYPE */

				if (strcmp(dent->d_name, ".") == 0 ||
				    strcmp(dent->d_name, "..") == 0)
					continue;
				qDebug("Selected interface '%s'",
				      dent->d_name);
				ctrl_iface = strdup(dent->d_name);
				break;
			}
			closedir(dir);
		}
#endif /* CONFIG_CTRL_IFACE_UNIX */
#ifdef CONFIG_CTRL_IFACE_NAMED_PIPE
		struct wpa_ctrl *ctrl;
		int ret;

		free(ctrl_iface);
		ctrl_iface = NULL;

		ctrl = wpa_ctrl_open(NULL);
		if (ctrl) {
			len = sizeof(buf) - 1;
			ret = wpa_ctrl_request(ctrl, "INTERFACES", 10, buf,
					       &len, NULL);
			if (ret >= 0) {
				buf[len] = '\0';
				pos = strchr(buf, '\n');
				if (pos)
					*pos = '\0';
				ctrl_iface = strdup(buf);
			}
			wpa_ctrl_close(ctrl);
		}
#endif /* CONFIG_CTRL_IFACE_NAMED_PIPE */
	}
//printf("9999999999999999999999999999\n");
    if (ctrl_iface == nullptr) {
#ifdef CONFIG_NATIVE_WINDOWS
		static bool first = true;
		if (first && !serviceRunning()) {
			first = false;
			if (QMessageBox::warning(
				    this, qAppName(),
				    tr("wpa_supplicant service is not "
				       "running.\n"
				       "Do you want to start it?"),
				    QMessageBox::Yes | QMessageBox::No) ==
			    QMessageBox::Yes)
				startService();
		}
#endif /* CONFIG_NATIVE_WINDOWS */
		return -1;
	}

#ifdef CONFIG_CTRL_IFACE_UNIX
	flen = strlen(ctrl_iface_dir) + strlen(ctrl_iface) + 2;
	cfile = (char *) malloc(flen);
    if (cfile == nullptr)
		return -1;
	snprintf(cfile, flen, "%s/%s", ctrl_iface_dir, ctrl_iface);
#else /* CONFIG_CTRL_IFACE_UNIX */
	flen = strlen(ctrl_iface) + 1;
	cfile = (char *) malloc(flen);
	if (cfile == NULL)
		return -1;
	snprintf(cfile, flen, "%s", ctrl_iface);
#endif /* CONFIG_CTRL_IFACE_UNIX */

	if (ctrl_conn) {
		wpa_ctrl_close(ctrl_conn);
        ctrl_conn = nullptr;
	}

	if (monitor_conn) {
        if(msgNotifier)
        {
            msgNotifier->disconnect(this);
            msgNotifier->deleteLater();
            msgNotifier = nullptr;
        }
		wpa_ctrl_detach(monitor_conn);
		wpa_ctrl_close(monitor_conn);
        monitor_conn = nullptr;
//        printf("111111111111111111111111open close\n");
	}

	qDebug("Trying to connect to '%s'", cfile);
	ctrl_conn = wpa_ctrl_open(cfile);
    if (ctrl_conn == nullptr) {
		free(cfile);
        qDebug() << "wpa_ctrl_open error";
		return -1;
	}
	monitor_conn = wpa_ctrl_open(cfile);
	free(cfile);
    if (monitor_conn == nullptr) {
		wpa_ctrl_close(ctrl_conn);
        qDebug() << "wpa_ctrl_open  error";
		return -1;
	}
	if (wpa_ctrl_attach(monitor_conn)) {
        qDebug("Failed to attach to wpa_supplicant");
		wpa_ctrl_close(monitor_conn);
        monitor_conn = nullptr;
		wpa_ctrl_close(ctrl_conn);
        ctrl_conn = nullptr;
		return -1;
	}

#if defined(CONFIG_CTRL_IFACE_UNIX) || defined(CONFIG_CTRL_IFACE_UDP)
    if(msgNotifier)
    {
        msgNotifier->deleteLater();
    }
	msgNotifier = new QSocketNotifier(wpa_ctrl_get_fd(monitor_conn),
					  QSocketNotifier::Read, this);
    connect(msgNotifier, SIGNAL(activated(int)), this, SLOT(receiveMsgs()));
#endif


	len = sizeof(buf) - 1;
	if (wpa_ctrl_request(ctrl_conn, "INTERFACES", 10, buf, &len, NULL) >=
	    0) {
		buf[len] = '\0';
		pos = buf;
		while (*pos) {
			pos2 = strchr(pos, '\n');
			if (pos2)
				*pos2 = '\0';
			if (pos2)
				pos = pos2 + 1;
			else
				break;
		}
	}

	len = sizeof(buf) - 1;
	if (wpa_ctrl_request(ctrl_conn, "GET_CAPABILITY eap", 18, buf, &len,
			     NULL) >= 0) {
		buf[len] = '\0';
	}

	return 0;
}


int WpaGui::ctrlRequest(const char *cmd, char *buf, size_t *buflen)
{
	int ret;

    if (ctrl_conn == nullptr)
		return -3;
    ret = wpa_ctrl_request(ctrl_conn, cmd, strlen(cmd), buf, buflen, nullptr);
	if (ret == -2)
		qDebug("'%s' command timed out.", cmd);
	else if (ret < 0)
		qDebug("'%s' command failed.", cmd);

	return ret;
}

int WpaGui::updateStatus()
{
    int wflev=6;
    char buf[2048], *start, *end;
	size_t len;

	pingsToStatusUpdate = 10;
	int i,j,k,l;
	len = sizeof(buf) - 1;
    if (ctrl_conn == nullptr || ctrlRequest("STATUS", buf, &len) < 0) {
//		qDebug("Could not get status from wpa_supplicant");
        return 6;
	}


	buf[len] = '\0';

    char *pairwise_cipher = nullptr, *group_cipher = nullptr;
    QString sta(buf);
	i=sta.indexOf("signal_level=");
	//end=pos;
	if (i) {
		start=buf+i;
		end=start;

		i=*(end+13);
		j=*(end+14);
		k=*(end+15);
		l=*(end+16);
		
		if(i=='\n'){
			wflev=0;
		}
		if(j=='\n'){
			wflev=(i-0x30);
		}
		if(k=='\n'){
			wflev=(i-0x30)*10+(j-0x30);
		}
		if(l=='\n'){
			wflev=(i-0x30)*100+(j-0x30)*10+(k-0x30);
			
			wflev=100;
		}
	}

	if (pairwise_cipher || group_cipher) {
		QString encr;
		if (pairwise_cipher && group_cipher &&
			strcmp(pairwise_cipher, group_cipher) != 0) {
			encr.append(pairwise_cipher);
			encr.append(" + ");
			encr.append(group_cipher);
		} else if (pairwise_cipher) {
			encr.append(pairwise_cipher);
		} else {
			encr.append(group_cipher);
			encr.append(" [group key only]");
		}
    }
    return wflev;
}


void WpaGui::updateNetworks()
{
	char buf[4096], *start, *end, *id, *ssid, *bssid, *flags;
	size_t len;
//	qDebug("updateNetworks ");

	if (!networkMayHaveChanged)
		return;

	if (ctrl_conn == NULL)
		return;
	qDebug("updateNetworks 2");

	len = sizeof(buf) - 1;
	if (ctrlRequest("LIST_NETWORKS", buf, &len) < 0)
		return;
	qDebug("updateNetworks 3");

	buf[len] = '\0';
	start = strchr(buf, '\n');
	if (start == NULL)
		return;
	start++;
	qDebug("updateNetworks %s ",buf);

	while (*start) {
		bool last = false;
		end = strchr(start, '\n');
		if (end == NULL) {
			last = true;
			end = start;
			while (end[0] && end[1])
				end++;
		}
		*end = '\0';

		id = start;
		ssid = strchr(id, '\t');
		if (ssid == NULL)
			break;
		*ssid++ = '\0';
		qDebug("ssid %s ",ssid);
		bssid = strchr(ssid, '\t');
		if (bssid == NULL)
			break;
		*bssid++ = '\0';
		qDebug("bssid %s ",bssid);
		flags = strchr(bssid, '\t');
		if (flags == NULL)
			break;
		*flags++ = '\0';

		if (strstr(flags, "[DISABLED][P2P-PERSISTENT]")) {
			if (last)
				break;
			start = end + 1;
			continue;
		}

		QString network(id);
		network.append(": ");
		network.append(ssid);

		if (last)
			break;
		start = end + 1;
	}
	networkMayHaveChanged = false;
}


void WpaGui::helpIndex()
{
	qDebug("helpIndex");
}


void WpaGui::helpContents()
{
	qDebug("helpContents");
}


void WpaGui::helpAbout()
{

}


void WpaGui::disconnect()
{
	char reply[10];
	size_t reply_len = sizeof(reply);
	ctrlRequest("DISCONNECT", reply, &reply_len);
	stopWpsRun(false);
}


void WpaGui::scan()
{
	char reply[10];
	size_t reply_len = sizeof(reply);   
	ctrlRequest("SCAN", reply, &reply_len);
}


void WpaGui::addNetwork(int auth,int encr)
{
	char reply[10], cmd[256];
	size_t reply_len;
	int id;
	NetworkConfig *nc = new NetworkConfig();
	if (nc == NULL)
		return;
	nc->setWpaGui(this);

	memset(reply, 0, sizeof(reply));
	reply_len = sizeof(reply) - 1;

	ctrlRequest("ADD_NETWORK", reply, &reply_len);
	if (reply[0] == 'F') {
		qDebug("Failed to add network to wpa_supplicant\n");
		return;
	}
	id = atoi(reply);
	qDebug("addNetwork %d\n",id);
	nc->setNetworkParam(id, "ssid", "ofzl",true);

    const char *key_mgmt = NULL, *proto = NULL, *pairwise = NULL;
	switch (auth) {
	case AUTH_NONE_OPEN:
	case AUTH_NONE_WEP:
	case AUTH_NONE_WEP_SHARED:
		key_mgmt = "NONE";
		break;
	case AUTH_IEEE8021X:
		key_mgmt = "IEEE8021X";
		break;
	case AUTH_WPA_PSK:
		key_mgmt = "WPA-PSK";
		proto = "WPA";
		break;
	case AUTH_WPA_EAP:
		key_mgmt = "WPA-EAP";
		proto = "WPA";
		break;
	case AUTH_WPA2_PSK:
		key_mgmt = "WPA-PSK";
		proto = "WPA2";
		break;
	case AUTH_WPA2_EAP:
		key_mgmt = "WPA-EAP";
		proto = "WPA2";
		break;
	}

/*	if (auth == AUTH_NONE_WEP_SHARED)
		nc->setNetworkParam(id, "auth_alg", "SHARED", false);
	else
		nc->setNetworkParam(id, "auth_alg", "OPEN", false);
*/
	if (auth == AUTH_WPA_PSK || auth == AUTH_WPA_EAP ||
	    auth == AUTH_WPA2_PSK || auth == AUTH_WPA2_EAP) {
		if (encr == 0)
			pairwise = "TKIP";
		else
			pairwise = "CCMP";
	}

	//nc->setNetworkParam(id, "bssid", "54:75:95:6d:a0:ad", false);
		if (key_mgmt){
			qDebug("key_mgmt %s\n",key_mgmt);
			nc->setNetworkParam(id, "key_mgmt", key_mgmt, false);}
		
		nc->setNetworkParam(networkid, "psk","ofzl123456789",true);
	//	nc->setNetworkParam(networkid, "password","ofzl123456789",true);
/*

	if (proto){
		nc->setNetworkParam(id, "proto", proto, false);
		qDebug("proto %s\n",proto);

	}

	if (pairwise) {
		qDebug("pairwise %s\n",pairwise);
		nc->setNetworkParam(id, "pairwise", pairwise, false);
		nc->setNetworkParam(id, "group", "TKIP CCMP", false);
	}
	QString prio;
	prio = prio.setNum(0);
	nc->setNetworkParam(id, "priority", prio.toLocal8Bit().constData(),
			false);
*/

	snprintf(cmd, sizeof(cmd), "ENABLE_NETWORK %d", id);
	reply_len = sizeof(reply);
	ctrlRequest(cmd, reply, &reply_len);
	if (strncmp(reply, "OK", 2) != 0) {
		qDebug("Failed to enable network in wpa_supplicant\n");
		/* Network was added, so continue anyway */
	}
	updateStatus();
//	connectB();
//	triggerUpdate();
//	updateStatus();
}

void WpaGui::updateResults()
{
	char reply[2048];
	size_t reply_len;
	int index;
	char cmd[20];
//	qDebug("updateResults \n");
	NetworkConfig *nc = new NetworkConfig();
	if (nc == NULL)
		return;
	nc->setWpaGui(this);

	index = 0;
	while (index < 100)
	{
		snprintf(cmd, sizeof(cmd), "BSS %d", index++);
		if (index > 100)
			return;
		
		reply_len = sizeof(reply) - 1;
		if (ctrlRequest(cmd, reply, &reply_len) < 0)
			return;
		reply[reply_len] = '\0';
		if(flagscan==0){
		//	qDebug("index %d \n",index);
		//	qDebug("reply %s",reply);
		}
		QString bss(reply);
		if (bss.isEmpty() || bss.startsWith("FAIL")){
			qDebug("bss.isEmpty");
			return;
		}
//        int i,j;
//        char *start, *end;
//        i=bss.indexOf("ofzl");
//        if(i>=0){
//            start=reply+i;
//            end=reply+i;
//            if(*(end+4)=='\n'){
//                qDebug("reply %s",reply);
//                qDebug("find id %d \n",index);
//                networkid=index-1;
//                qDebug("networkid %d \n",networkid);
//                j=0;
//                while (!j)
//                {
//                    j=bss.indexOf("WPA2-PSK");
//                    if(j>0){
//                        wfauth=AUTH_WPA2_PSK;
//                        break;
//                    }
//                    j=bss.indexOf("WPA-PSK");
//                    if(j>0){
//                        wfauth=AUTH_WPA_PSK;
//                        break;
//                    }

//                }
//                qDebug("wfauth %d \n",wfauth);

//            }
//        }
	}
	qDebug("updateResults e\n");
	flagscan=1;
}


void WpaGui::eventHistory()
{


}


void WpaGui::ping()
{
	char buf[10];
	size_t len;

#ifdef CONFIG_CTRL_IFACE_NAMED_PIPE
	/*
	 * QSocketNotifier cannot be used with Windows named pipes, so use a
	 * timer to check for received messages for now. This could be
	 * optimized be doing something specific to named pipes or Windows
	 * events, but it is not clear what would be the best way of doing that
	 * in Qt.
	 */
	receiveMsgs();
#endif /* CONFIG_CTRL_IFACE_NAMED_PIPE */


	len = sizeof(buf) - 1;
	if (ctrlRequest("PING", buf, &len) < 0) {
		qDebug("PING failed - trying to reconnect");
		if (openCtrlConnection(ctrl_iface) >= 0) {
			qDebug("Reconnected successfully");
			pingsToStatusUpdate = 0;
		}
	}

	pingsToStatusUpdate--;
	if (pingsToStatusUpdate <= 0) {
		updateStatus();
		updateNetworks();
	}

#ifndef CONFIG_CTRL_IFACE_NAMED_PIPE
	/* Use less frequent pings and status updates when the main window is
	 * hidden (running in taskbar). */
#endif /* CONFIG_CTRL_IFACE_NAMED_PIPE */
}


void WpaGui::signalMeterUpdate()
{
	char reply[128];
	size_t reply_len = sizeof(reply);

	ctrlRequest("SIGNAL_POLL", reply, &reply_len);
	qDebug("signalMeterUpdate");
	/*
	 * NOTE: The code below assumes, that the unit of the value returned
	 * by the SIGNAL POLL request is dBm. It might not be true for all
	 * wpa_supplicant drivers.
	 */

	/*
	 * Calibration is based on "various Internet sources". Nonetheless,
	 * it seems to be compatible with the Windows 8.1 strength meter -
	 * tested on Intel Centrino Advanced-N 6235.
	 */
}


static int str_match(const char *a, const char *b)
{
	return strncmp(a, b, strlen(b)) == 0;
}


void WpaGui::processMsg(char *msg)
{
//    qDebug() << "==============processMsg:" << msg;
	char *pos = msg, *pos2;
	int priority = 2;

	if (*pos == '<') {
		/* skip priority */
		pos++;
		priority = atoi(pos);
		pos = strchr(pos, '>');
		if (pos)
			pos++;
		else
			pos = msg;
	}
	/* Update last message with truncated version of the event */
	if (strncmp(pos, "CTRL-", 5) == 0) {
		pos2 = strchr(pos, str_match(pos, WPA_CTRL_REQ) ? ':' : ' ');
		if (pos2)
			pos2++;
		else
			pos2 = pos;
	} else
		pos2 = pos;
	QString lastmsg = pos2;
	lastmsg.truncate(40);

	pingsToStatusUpdate = 0;
	networkMayHaveChanged = true;
	if (str_match(pos, WPA_CTRL_REQ))
		processCtrlReq(pos + strlen(WPA_CTRL_REQ));
	else if (str_match(pos, WPA_EVENT_SCAN_RESULTS)){
		updateResults();
	}	
	else if (str_match(pos, WPA_EVENT_DISCONNECTED)){
		showTrayMessage(QSystemTrayIcon::Information, 3,
                        tr("Disconnected from network."));
        currentIconType = TrayIconOffline;
        emit disconnected();
//        qDebug("111111111111111111111111111111111111111111111111111WPA_EVENT_DISCONNECTED");
	}
	else if (str_match(pos, WPA_EVENT_CONNECTED)) {
        showTrayMessage(QSystemTrayIcon::Information, 3,
                        tr("Connection to network established."));
        QTimer::singleShot(5 * 1000, this, SLOT(showTrayStatus()));
        stopWpsRun(true);
        currentIconType = TrayIconConnected;
        emit connected();
//        qDebug("222222222222222222222222222222222222222222222222WPA_EVENT_CONNECTED");
	} else if (str_match(pos, WPS_EVENT_AP_AVAILABLE_PBC)) {
		
	} else if (str_match(pos, WPS_EVENT_AP_AVAILABLE_PIN)) {

	} else if (str_match(pos, WPS_EVENT_AP_AVAILABLE_AUTH)) {

	} else if (str_match(pos, WPS_EVENT_AP_AVAILABLE)) {
		qDebug("WPS AP detected");
	} else if (str_match(pos, WPS_EVENT_OVERLAP)) {
		qDebug("PBC mode overlap detected");
	} else if (str_match(pos, WPS_EVENT_CRED_RECEIVED)) {
		qDebug("Network configuration received");
	} else if (str_match(pos, WPA_EVENT_EAP_METHOD)) {
		if (strstr(pos, "(WSC)"))
			qDebug("Registration started");
	} else if (str_match(pos, WPS_EVENT_M2D)) {
		qDebug("Registrar does not yet know PIN");
	} else if (str_match(pos, WPS_EVENT_FAIL)) {
		qDebug("Registration failed");
	} else if (str_match(pos, WPS_EVENT_SUCCESS)) {
		qDebug("Registration succeeded");
	}
    QString result = pos;
    if (result.startsWith("Authentication with") &&
            result.endsWith("timed out.")) {
        removeNetwork();
        emit pskError();
    }
}


void WpaGui::processCtrlReq(const char *req)
{

}


void WpaGui::receiveMsgs()
{
	char buf[256];
	size_t len;

	while (monitor_conn && wpa_ctrl_pending(monitor_conn) > 0) {
		len = sizeof(buf) - 1;
		if (wpa_ctrl_recv(monitor_conn, buf, &len) == 0) {
			buf[len] = '\0';
			processMsg(buf);
		}
	}
}


void WpaGui::connectB()
{
	char reply[10];
	size_t reply_len = sizeof(reply);
	ctrlRequest("REASSOCIATE", reply, &reply_len);
}


void WpaGui::selectNetwork()
{
	QString cmd("0");
	char reply[10];
	size_t reply_len = sizeof(reply);

	cmd.prepend("SELECT_NETWORK ");
	ctrlRequest(cmd.toLocal8Bit().constData(), reply, &reply_len);
	triggerUpdate();

}


void WpaGui::enableNetwork(char *ssid, char *psk, int auth)
{
    removeNetwork();
    currentIconType = TrayIconAcquiring;
	char reply[10], cmd[256];
	int id;
	size_t reply_len = sizeof(reply);
    qDebug("ssid %s \n",ssid);
    qDebug("psk %s \n",psk);
    qDebug("auth %d\n",auth);

    NetworkConfig *nc = new NetworkConfig();
    if (nc == nullptr)
        return;
    nc->setWpaGui(this);

    memset(reply, 0, 10);
    reply_len = sizeof(reply) - 1;

    int ret = ctrlRequest("ADD_NETWORK", reply, &reply_len);
//    qDebug() << "===========================" << ret;
    if (reply[0] == 'F') {
        qDebug("Failed to add network to wpa_supplicant\n");
		return;
	}
    for(int i = 0;i < 10;i++)
    {
        qDebug() << reply[i];
    }
	id = atoi(reply);
	qDebug("addNetwork %d\n",id);
    nc->setNetworkParam(id, "ssid", ssid, true);

    const char *key_mgmt = nullptr, *proto = nullptr, *pairwise = nullptr;
	switch (auth) {
	case AUTH_NONE_OPEN:
	case AUTH_NONE_WEP:
	case AUTH_NONE_WEP_SHARED:
		key_mgmt = "NONE";
		break;
	case AUTH_IEEE8021X:
		key_mgmt = "IEEE8021X";
		break;
	case AUTH_WPA_PSK:
		key_mgmt = "WPA-PSK";
		proto = "WPA";
		break;
	case AUTH_WPA_EAP:
		key_mgmt = "WPA-EAP";
		proto = "WPA";
		break;
	case AUTH_WPA2_PSK:
		key_mgmt = "WPA-PSK";
		proto = "WPA2";
		break;
	case AUTH_WPA2_EAP:
		key_mgmt = "WPA-EAP";
		proto = "WPA2";
		break;
	}

    if (key_mgmt)
    {
        qDebug("key_mgmt %s\n",key_mgmt);
        nc->setNetworkParam(id, "key_mgmt", key_mgmt, true);
    }
    nc->setNetworkParam(id, "proto", "RSN WPA WPA2", true);
    nc->setNetworkParam(id, "pairwise", "TKIP CCMP", true);
    nc->setNetworkParam(id, "psk",psk ,sizeof(psk)!=64);
	snprintf(cmd, sizeof(cmd), "ENABLE_NETWORK %d", id);
	reply_len = sizeof(reply);
	ctrlRequest(cmd, reply, &reply_len);
    if (strncmp(reply, "OK", 2) != 0)
    {
		qDebug("Failed to enable network in wpa_supplicant\n");
		/* Network was added, so continue anyway */
	}


}


void WpaGui::disableNetwork()
{
	QString cmd("all");
	char reply[10];
	size_t reply_len = sizeof(reply);

	cmd.prepend("DISABLE_NETWORK ");
	ctrlRequest(cmd.toLocal8Bit().constData(), reply, &reply_len);
	triggerUpdate();

}


void WpaGui::editNetwork()
{
	NetworkConfig *nc = new NetworkConfig();
	if (nc == NULL)
		return;
	nc->setWpaGui(this);
	nc->paramsFromConfig(networkid);

}


void WpaGui::editSelectedNetwork()
{

}


void WpaGui::editListedNetwork()
{

}


void WpaGui::triggerUpdate()
{
//	qDebug("triggerUpdate");
	updateStatus();
	networkMayHaveChanged = true;
	updateNetworks();
//	qDebug("triggerUpdate e");

}



void WpaGui::removeNetwork()
{
	QString cmd("all");
	char reply[10];
	size_t reply_len = sizeof(reply);
	flagadd=0;


	cmd.prepend("REMOVE_NETWORK ");
	ctrlRequest(cmd.toLocal8Bit().constData(), reply, &reply_len);
	triggerUpdate();

}


void WpaGui::removeSelectedNetwork()
{

}


void WpaGui::removeListedNetwork()
{

}


void WpaGui::enableAllNetworks()
{
	QString sel("all");
//	enableNetwork();
}


void WpaGui::disableAllNetworks()
{
	QString sel("all");
	disableNetwork();
}


void WpaGui::removeAllNetworks()
{
	QString sel("all");
	removeNetwork();
}


int WpaGui::getNetworkDisabled(const QString &sel)
{
	return -1;
}


void WpaGui::updateNetworkDisabledStatus()
{

}


void WpaGui::enableListedNetwork(bool enabled)
{

}


void WpaGui::disableListedNetwork(bool disabled)
{

}


void WpaGui::saveConfig()
{

}


void WpaGui::selectAdapter( const QString & sel )
{

}


void WpaGui::createTrayIcon(bool trayOnly)
{

}


void WpaGui::showTrayMessage(QSystemTrayIcon::MessageIcon type, int sec,
			     const QString & msg)
{

}


void WpaGui::trayActivated(QSystemTrayIcon::ActivationReason how)
 {

}


void WpaGui::showTrayStatus()
{

}


void WpaGui::updateTrayToolTip(const QString &msg)
{
}


void WpaGui::updateTrayIcon(TrayIconType type)
{

}


QIcon WpaGui::loadThemedIcon(const QStringList &names,
			     const QIcon &fallback)
{

}




void WpaGui::wpsDialog()
{
}


void WpaGui::peersDialog()
{
}


void WpaGui::tabChanged(int index)
{
	if (index != 2)
		return;

	if (wpsRunning)
		return;

}


void WpaGui::wpsPbc()
{
	char reply[20];
	size_t reply_len = sizeof(reply);

	if (ctrlRequest("WPS_PBC", reply, &reply_len) < 0)
		return;

}


void WpaGui::wpsGeneratePin()
{
	char reply[20];
	size_t reply_len = sizeof(reply) - 1;

	if (ctrlRequest("WPS_PIN any", reply, &reply_len) < 0)
		return;

	reply[reply_len] = '\0';

}


void WpaGui::setBssFromScan(const QString &bssid)
{
	bssFromScan = bssid;
}


void WpaGui::wpsApPinChanged(const QString &text)
{

}


void WpaGui::wpsApPin()
{


}


void WpaGui::stopWpsRun(bool success)
{

}


#ifdef CONFIG_NATIVE_WINDOWS

#ifndef WPASVC_NAME
#define WPASVC_NAME TEXT("wpasvc")
#endif

class ErrorMsg : public QMessageBox {
public:
	ErrorMsg(QWidget *parent, DWORD last_err = GetLastError());
	void showMsg(QString msg);
private:
	DWORD err;
};

ErrorMsg::ErrorMsg(QWidget *parent, DWORD last_err) :
	QMessageBox(parent), err(last_err)
{
	setWindowTitle(tr("wpa_gui error"));
	setIcon(QMessageBox::Warning);
}

void ErrorMsg::showMsg(QString msg)
{
	LPTSTR buf;

	setText(msg);
	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			  FORMAT_MESSAGE_FROM_SYSTEM,
			  NULL, err, 0, (LPTSTR) (void *) &buf,
			  0, NULL) > 0) {
		QString msg = QString::fromWCharArray(buf);
		setInformativeText(QString("[%1] %2").arg(err).arg(msg));
		LocalFree(buf);
	} else {
		setInformativeText(QString("[%1]").arg(err));
	}

	exec();
}


void WpaGui::startService()
{
	SC_HANDLE svc, scm;

	scm = OpenSCManager(0, 0, SC_MANAGER_CONNECT);
	if (!scm) {
		ErrorMsg(this).showMsg(tr("OpenSCManager failed"));
		return;
	}

	svc = OpenService(scm, WPASVC_NAME, SERVICE_START);
	if (!svc) {
		ErrorMsg(this).showMsg(tr("OpenService failed"));
		CloseServiceHandle(scm);
		return;
	}

	if (!StartService(svc, 0, NULL)) {
		ErrorMsg(this).showMsg(tr("Failed to start wpa_supplicant "
				       "service"));
	}

	CloseServiceHandle(svc);
	CloseServiceHandle(scm);
}


void WpaGui::stopService()
{
	SC_HANDLE svc, scm;
	SERVICE_STATUS status;

	scm = OpenSCManager(0, 0, SC_MANAGER_CONNECT);
	if (!scm) {
		ErrorMsg(this).showMsg(tr("OpenSCManager failed"));
		return;
	}

	svc = OpenService(scm, WPASVC_NAME, SERVICE_STOP);
	if (!svc) {
		ErrorMsg(this).showMsg(tr("OpenService failed"));
		CloseServiceHandle(scm);
		return;
	}

	if (!ControlService(svc, SERVICE_CONTROL_STOP, &status)) {
		ErrorMsg(this).showMsg(tr("Failed to stop wpa_supplicant "
				       "service"));
	}

	CloseServiceHandle(svc);
	CloseServiceHandle(scm);
}


bool WpaGui::serviceRunning()
{
	SC_HANDLE svc, scm;
	SERVICE_STATUS status;
	bool running = false;

	scm = OpenSCManager(0, 0, SC_MANAGER_CONNECT);
	if (!scm) {
		qDebug("OpenSCManager failed: %d", (int) GetLastError());
		return false;
	}

	svc = OpenService(scm, WPASVC_NAME, SERVICE_QUERY_STATUS);
	if (!svc) {
		qDebug("OpenService failed: %d", (int) GetLastError());
		CloseServiceHandle(scm);
		return false;
	}

	if (QueryServiceStatus(svc, &status)) {
		if (status.dwCurrentState != SERVICE_STOPPED)
			running = true;
	}

	CloseServiceHandle(svc);
	CloseServiceHandle(scm);

	return running;
}

#endif /* CONFIG_NATIVE_WINDOWS */


void WpaGui::addInterface()
{

}


#ifndef QT_NO_SESSIONMANAGER
void WpaGui::saveState()
{
}
#endif


void WpaGui::reOpenWpa()
{
//    if(0 == type)
//    {
//        system("himm 0x120D3010 0x00 && rmmod 8188eu && insmod ko/8188eu.ko  && sleep 1 && himm 0x120D3010 0x04 && sleep 2 &&"
//               " wpa_supplicant -B -Dwext -iwlan0 -c /mnt/usr/wireless/wpa.conf && udhcpc -i wlan0 &");
//    }
//    else {
//        system("rmmod 8188eu && insmod ko/8188eu.ko  && sleep 1 && wpa_supplicant -B -Dwext -iwlan0 -c /mnt/usr/wireless/wpa.conf");
//    }
//    qDebug() << "=======================" << ctrl_iface;
//    sleep(1);
    if (openCtrlConnection(ctrl_iface) < 0)
    {
        qDebug("reOpenWpa Failed to open control connection to "
              "wpa_supplicant.");
    }
}

void WpaGui::setState(TrayIconType type)
{
    currentIconType = type;
}

WpaGui::TrayIconType WpaGui::state() const
{
    return currentIconType;
}
