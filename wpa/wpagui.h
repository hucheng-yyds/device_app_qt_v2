/*
 * wpa_gui - WpaGui class
 * Copyright (c) 2005-2006, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef WPAGUI_H
#define WPAGUI_H

#include <QSystemTrayIcon>
#include <QObject>
#include <QTimer>
#include <QAction>
#include <QSocketNotifier>
#include "wpamsg.h"

//class UserDataRequest;
enum {
	AUTH_NONE_OPEN,
	AUTH_NONE_WEP,
	AUTH_NONE_WEP_SHARED,
	AUTH_IEEE8021X,
	AUTH_WPA_PSK,
	AUTH_WPA_EAP,
	AUTH_WPA2_PSK,
	AUTH_WPA2_EAP
};


class WpaGui : public QObject
{
	Q_OBJECT

public:

	enum TrayIconType {
		TrayIconOffline = 0,
		TrayIconAcquiring,
		TrayIconConnected,
		TrayIconSignalNone,
		TrayIconSignalWeak,
		TrayIconSignalOk,
		TrayIconSignalGood,
		TrayIconSignalExcellent,
	};
    Q_ENUM(TrayIconType)

	WpaGui();
	~WpaGui();

	virtual int ctrlRequest(const char *cmd, char *buf, size_t *buflen);
	virtual void triggerUpdate();
	virtual void editNetwork();
	virtual void removeNetwork();
    virtual void enableNetwork(char *ssid, char *psk, int auth);
	virtual void disableNetwork();
	virtual int getNetworkDisabled(const QString &sel);
	void setBssFromScan(const QString &bssid);
#ifndef QT_NO_SESSIONMANAGER
	void saveState();
#endif
    TrayIconType state() const;
    void setState(TrayIconType type);
    void reOpenWpa();

public slots:
	virtual void parse_argv();
    virtual int updateStatus();
	virtual void updateNetworks();
	virtual void helpIndex();
	virtual void helpContents();
	virtual void helpAbout();
	virtual void disconnect();
	virtual void scan();
	virtual void eventHistory();
	virtual void ping();
	virtual void signalMeterUpdate();
	virtual void processMsg(char *msg);
	virtual void processCtrlReq(const char *req);
	virtual void receiveMsgs();
	virtual void connectB();
	virtual void selectNetwork();
	virtual void editSelectedNetwork();
	virtual void editListedNetwork();
	virtual void removeSelectedNetwork();
	virtual void removeListedNetwork();
	virtual void addNetwork(int auth,int encr);
	virtual void enableAllNetworks();
	virtual void disableAllNetworks();
	virtual void removeAllNetworks();
	virtual void saveConfig();
	virtual void selectAdapter(const QString &sel);
	virtual void updateNetworkDisabledStatus();
	virtual void enableListedNetwork(bool);
	virtual void disableListedNetwork(bool);
	virtual void showTrayMessage(QSystemTrayIcon::MessageIcon type,
				     int sec, const QString &msg);
	virtual void showTrayStatus();
	virtual void updateTrayIcon(TrayIconType type);
	virtual void updateTrayToolTip(const QString &msg);
	virtual QIcon loadThemedIcon(const QStringList &names,
				     const QIcon &fallback);
	virtual void wpsDialog();
	virtual void peersDialog();
	virtual void tabChanged(int index);
	virtual void wpsPbc();
	virtual void wpsGeneratePin();
	virtual void wpsApPinChanged(const QString &text);
	virtual void wpsApPin();
#ifdef CONFIG_NATIVE_WINDOWS
	virtual void startService();
	virtual void stopService();
#endif /* CONFIG_NATIVE_WINDOWS */
	virtual void addInterface();

protected slots:
	virtual void languageChange();
	virtual void trayActivated(QSystemTrayIcon::ActivationReason how);

private:
	bool networkMayHaveChanged;
	char *ctrl_iface;
	struct wpa_ctrl *ctrl_conn;
	QSocketNotifier *msgNotifier;
	QTimer *timer;
	int pingsToStatusUpdate;
	WpaMsgList msgs;
	char *ctrl_iface_dir;
	struct wpa_ctrl *monitor_conn;
	QAction *disconnectAction;
	QAction *reconnectAction;
	QAction *eventAction;
	QAction *scanAction;
	QAction *statAction;
	QAction *showAction;
	QAction *hideAction;
	QAction *quitAction;
	QMenu *tray_menu;
	QSystemTrayIcon *tray_icon;
	TrayIconType currentIconType;
	void createTrayIcon(bool);
	bool ackTrayIcon;
	bool startInTray;
	bool quietMode;
	void updateResults();

	int openCtrlConnection(const char *ifname);

	bool wpsRunning;

	QString bssFromScan;

    void stopWpsRun(bool success);
#ifdef CONFIG_NATIVE_WINDOWS
	QAction *fileStartServiceAction;
	QAction *fileStopServiceAction;

	bool serviceRunning();
#endif /* CONFIG_NATIVE_WINDOWS */
signals:
    void connected();
    void disconnected();
    void pskError();
};

#endif /* WPAGUI_H */
