/*
 * wpa_gui - NetworkConfig class
 * Copyright (c) 2005-2006, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include <cstdio>
#include <QMessageBox>

#include "networkconfig.h"
#include "wpagui.h"



#define WPA_GUI_KEY_DATA "[key is configured]"


NetworkConfig::NetworkConfig()
{
	wpagui = NULL;
	new_network = false;
}


NetworkConfig::~NetworkConfig()
{
}


void NetworkConfig::languageChange()
{
}


//void NetworkConfig::paramsFromScanResults(QTreeWidgetItem *sel)
//{

//}


void NetworkConfig::authChanged(int sel)
{

}


void NetworkConfig::eapChanged(int sel)
{

}


void NetworkConfig::addNetwork()
{
}


void NetworkConfig::setWpaGui(WpaGui *_wpagui)
{
	wpagui = _wpagui;
}


int NetworkConfig::setNetworkParam(int id, const char *field,
				   const char *value, bool quote)
{
	char reply[10], cmd[256];
	size_t reply_len;
	snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s %s%s%s",
		 id, field, quote ? "\"" : "", value, quote ? "\"" : "");
	reply_len = sizeof(reply);
	wpagui->ctrlRequest(cmd, reply, &reply_len);
	return strncmp(reply, "OK", 2) == 0 ? 0 : -1;
}


void NetworkConfig::encrChanged(const QString &)
{
}


void NetworkConfig::wepEnabled(bool enabled)
{

}


//void NetworkConfig::writeWepKey(int network_id, QLineEdit *edit, int id)
//{

//}


static int key_value_isset(const char *reply, size_t reply_len)
{
    return reply_len > 0 && (reply_len < 4 || memcmp(reply, "FAIL", 4) != 0);
}


void NetworkConfig::paramsFromConfig(int network_id)
{
	int i, res;

	edit_network_id = network_id;
	getEapCapa();

	char reply[1024], cmd[256], *pos;
	size_t reply_len;

	snprintf(cmd, sizeof(cmd), "GET_NETWORK %d ssid", network_id);
	reply_len = sizeof(reply) - 1;
	if (wpagui->ctrlRequest(cmd, reply, &reply_len) >= 0 &&
	    reply_len >= 2 && reply[0] == '"') {
		reply[reply_len] = '\0';
		pos = strchr(reply + 1, '"');
		if (pos)
			*pos = '\0';
		qDebug("ssid %s",reply);
	}

	snprintf(cmd, sizeof(cmd), "GET_NETWORK %d proto", network_id);
	reply_len = sizeof(reply) - 1;
	int wpa = 0;
	if (wpagui->ctrlRequest(cmd, reply, &reply_len) >= 0) {
		reply[reply_len] = '\0';
		if (strstr(reply, "RSN") || strstr(reply, "WPA2"))
			wpa = 2;
		else if (strstr(reply, "WPA"))
			wpa = 1;
		qDebug("proto %d",wpa);
	}

	int auth = AUTH_NONE_OPEN, encr = 0;
	snprintf(cmd, sizeof(cmd), "GET_NETWORK %d key_mgmt", network_id);
	reply_len = sizeof(reply) - 1;
	if (wpagui->ctrlRequest(cmd, reply, &reply_len) >= 0) {
		reply[reply_len] = '\0';
		if (strstr(reply, "WPA-EAP"))
			auth = wpa & 2 ? AUTH_WPA2_EAP : AUTH_WPA_EAP;
		else if (strstr(reply, "WPA-PSK"))
			auth = wpa & 2 ? AUTH_WPA2_PSK : AUTH_WPA_PSK;
		else if (strstr(reply, "IEEE8021X")) {
			auth = AUTH_IEEE8021X;
			encr = 1;
		}
		qDebug("auth %d",auth);
	}

	snprintf(cmd, sizeof(cmd), "GET_NETWORK %d pairwise", network_id);
	reply_len = sizeof(reply) - 1;
	if (wpagui->ctrlRequest(cmd, reply, &reply_len) >= 0) {
		reply[reply_len] = '\0';
		if (strstr(reply, "CCMP") && auth != AUTH_NONE_OPEN &&
		    auth != AUTH_NONE_WEP && auth != AUTH_NONE_WEP_SHARED)
			encr = 1;
		else if (strstr(reply, "TKIP"))
			encr = 0;
		else if (strstr(reply, "WEP"))
			encr = 1;
		else
			encr = 0;
		qDebug("encr %d",encr);
	}

	snprintf(cmd, sizeof(cmd), "GET_NETWORK %d psk", network_id);
	reply_len = sizeof(reply) - 1;
	res = wpagui->ctrlRequest(cmd, reply, &reply_len);
	if (res >= 0 && reply_len >= 2 && reply[0] == '"') {
		reply[reply_len] = '\0';
		pos = strchr(reply + 1, '"');
		if (pos)
			*pos = '\0';
		qDebug("psk %s",pos);
	} else if (res >= 0 && key_value_isset(reply, reply_len)) {
		qDebug(WPA_GUI_KEY_DATA);
	}

	snprintf(cmd, sizeof(cmd), "GET_NETWORK %d identity", network_id);
	reply_len = sizeof(reply) - 1;
	if (wpagui->ctrlRequest(cmd, reply, &reply_len) >= 0 &&
	    reply_len >= 2 && reply[0] == '"') {
		reply[reply_len] = '\0';
		pos = strchr(reply + 1, '"');
		if (pos)
			*pos = '\0';
		qDebug("identity %s",reply);
	}

	snprintf(cmd, sizeof(cmd), "GET_NETWORK %d password", network_id);
	reply_len = sizeof(reply) - 1;
	res = wpagui->ctrlRequest(cmd, reply, &reply_len);
	if (res >= 0 && reply_len >= 2 && reply[0] == '"') {
		reply[reply_len] = '\0';
		pos = strchr(reply + 1, '"');
		if (pos)
			*pos = '\0';
		qDebug("password %s",reply);
	} else if (res >= 0 && key_value_isset(reply, reply_len)) {
		qDebug(WPA_GUI_KEY_DATA);
	}

	snprintf(cmd, sizeof(cmd), "GET_NETWORK %d ca_cert", network_id);
	reply_len = sizeof(reply) - 1;
	if (wpagui->ctrlRequest(cmd, reply, &reply_len) >= 0 &&
	    reply_len >= 2 && reply[0] == '"') {
		reply[reply_len] = '\0';
		pos = strchr(reply + 1, '"');
		if (pos)
			*pos = '\0';
		qDebug("ca_cert %s",reply);
	}

	enum { NO_INNER, PEAP_INNER, TTLS_INNER, FAST_INNER } eap = NO_INNER;
	snprintf(cmd, sizeof(cmd), "GET_NETWORK %d eap", network_id);
	reply_len = sizeof(reply) - 1;
	if (wpagui->ctrlRequest(cmd, reply, &reply_len) >= 0 &&
	    reply_len >= 1) {
		reply[reply_len] = '\0';
		qDebug("eap %s",reply);
	}

	if (eap != NO_INNER) {
		snprintf(cmd, sizeof(cmd), "GET_NETWORK %d phase2",
			 network_id);
		reply_len = sizeof(reply) - 1;
		if (wpagui->ctrlRequest(cmd, reply, &reply_len) >= 0 &&
		    reply_len >= 1) {
			reply[reply_len] = '\0';
			//eapChanged(eapSelect->currentIndex());
			qDebug("eapChanged %s",reply);
		} else
			eap = NO_INNER;
	}

	char *val;
	val = reply + 1;
	while (*(val + 1))
		val++;
	if (*val == '"')
		*val = '\0';

	switch (eap) {
	case PEAP_INNER:
		if (strncmp(reply, "\"auth=", 6))
			break;
		val = reply + 2;
		memcpy(val, "EAP-", 4);
		break;
	case TTLS_INNER:
		if (strncmp(reply, "\"autheap=", 9) == 0) {
			val = reply + 5;
			memcpy(val, "EAP-", 4);
		} else if (strncmp(reply, "\"auth=", 6) == 0)
			val = reply + 6;
		break;
	case FAST_INNER:
		if (strncmp(reply, "\"auth=", 6))
			break;
		if (strcmp(reply + 6, "GTC auth=MSCHAPV2") == 0) {
			val = (char *) "GTC(auth) + MSCHAPv2(prov)";
			break;
		}
		val = reply + 2;
		memcpy(val, "EAP-", 4);
		break;
	case NO_INNER:
		break;
	}



	for (i = 0; i < 4; i++) {
		snprintf(cmd, sizeof(cmd), "GET_NETWORK %d wep_key%d",
			 network_id, i);
		reply_len = sizeof(reply) - 1;
		res = wpagui->ctrlRequest(cmd, reply, &reply_len);
		if (res >= 0 && reply_len >= 2 && reply[0] == '"') {
			reply[reply_len] = '\0';
			pos = strchr(reply + 1, '"');
			if (pos)
				*pos = '\0';
			if (auth == AUTH_NONE_OPEN || auth == AUTH_IEEE8021X) {
				if (auth == AUTH_NONE_OPEN)
					auth = AUTH_NONE_WEP;
				encr = 1;
			}
			qDebug("wep_key %s",reply);
		} else if (res >= 0 && key_value_isset(reply, reply_len)) {
			if (auth == AUTH_NONE_OPEN || auth == AUTH_IEEE8021X) {
				if (auth == AUTH_NONE_OPEN)
					auth = AUTH_NONE_WEP;
				encr = 1;
			}
			qDebug(WPA_GUI_KEY_DATA);
		}
	}

	if (auth == AUTH_NONE_WEP) {
		snprintf(cmd, sizeof(cmd), "GET_NETWORK %d auth_alg",
			 network_id);
		reply_len = sizeof(reply) - 1;
		if (wpagui->ctrlRequest(cmd, reply, &reply_len) >= 0) {
			reply[reply_len] = '\0';
			if (strcmp(reply, "SHARED") == 0)
				auth = AUTH_NONE_WEP_SHARED;
		}
	}

	snprintf(cmd, sizeof(cmd), "GET_NETWORK %d wep_tx_keyidx", network_id);
	reply_len = sizeof(reply) - 1;
	if (wpagui->ctrlRequest(cmd, reply, &reply_len) >= 0 && reply_len >= 1)
	{
		reply[reply_len] = '\0';
		qDebug("wep_tx_keyidx %s",reply);
	}

	snprintf(cmd, sizeof(cmd), "GET_NETWORK %d id_str", network_id);
	reply_len = sizeof(reply) - 1;
	if (wpagui->ctrlRequest(cmd, reply, &reply_len) >= 0 &&
	    reply_len >= 2 && reply[0] == '"') {
		reply[reply_len] = '\0';
		pos = strchr(reply + 1, '"');
		if (pos)
			*pos = '\0';
		qDebug("id_str %s",reply);
	}

	snprintf(cmd, sizeof(cmd), "GET_NETWORK %d priority", network_id);
	reply_len = sizeof(reply) - 1;
	if (wpagui->ctrlRequest(cmd, reply, &reply_len) >= 0 && reply_len >= 1)
	{
		reply[reply_len] = '\0';
		qDebug("priority %s",reply);
	}

	authChanged(auth);
	wepEnabled(auth == AUTH_NONE_WEP || auth == AUTH_NONE_WEP_SHARED);

}


void NetworkConfig::removeNetwork()
{
	char reply[10], cmd[256];
	size_t reply_len;

	snprintf(cmd, sizeof(cmd), "REMOVE_NETWORK %d", edit_network_id);
	reply_len = sizeof(reply);
}


void NetworkConfig::newNetwork()
{
	new_network = true;
	getEapCapa();
}


void NetworkConfig::getEapCapa()
{
	char reply[256];
	size_t reply_len;

	if (wpagui == NULL)
		return;

	reply_len = sizeof(reply) - 1;
	if (wpagui->ctrlRequest("GET_CAPABILITY eap", reply, &reply_len) < 0)
		return;
	reply[reply_len] = '\0';

}


void NetworkConfig::useWps()
{
	if (wpagui == NULL)
		return;
	wpagui->setBssFromScan(bssid);
}
