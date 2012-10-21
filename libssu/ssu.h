/**
 * @file ssu.h
 * @copyright 2012 Jolla Ltd.
 * @author Bernd Wachter <bernd.wachter@jollamobile.com>
 * @date 2012
 */

#ifndef _Ssu_H
#define _Ssu_H

#include <QObject>
#include <QSettings>
#include <QDebug>

#include <QtNetwork>

#include <QtXml/QDomDocument>

class Ssu: public QObject {
    Q_OBJECT

  public:
    Ssu();
    /**
     * Find a username/password pair for the given scope
     * @return a QPair with username and password, or an empty QPair if scope is invalid
     */
    QPair<QString, QString> credentials(QString scope);
    /**
     * Get the scope for a repository, taking into account different scopes for
     * release and RnD repositories
     *
     * Please note that variable scope is not yet implemented -- one default scope is
     * read from the configuration file.
     *
     * @return a string containing the scope; it can be used to look up login credentials using  credentials()
     */
    QString credentialsScope(QString repoName, bool rndRepo=false);
    /**
     * Try to find the device family for the system this is running on
     */
    Q_INVOKABLE QString deviceFamily();
    /**
     * Try to find out ond what kind of system this is running
     */
    Q_INVOKABLE QString deviceModel();
    /**
     * Calculate the device ID used in SSU requests
     * @return QSystemDeviceInfo::imei(), if available, or QSystemDeviceInfo::uniqueDeviceID()
     */
    Q_INVOKABLE QString deviceUid();
    /**
     * Returns if the last operation was successful
     * @retval true last operation was successful
     * @retval false last operation failed, you should check lastError() for details
     */
    Q_INVOKABLE bool error();
    /**
     * Get the current flavour when RnD repositories are used
     * @return current flavour (usually something like testing, release, ..)
     */
    Q_INVOKABLE QString flavour();
    /**
     * Return devices RND registration status
     * @retval true device is registered
     * @retval false device is not registered
     */
    Q_INVOKABLE bool isRegistered();
    /**
     * Return the date/time when the credentials to access internal
     * SSU servers were updated the last time
     */
    Q_INVOKABLE QDateTime lastCredentialsUpdate();
    /**
     * Return an error message for the last error encountered. The message
     * will not be cleared, check error() to see if the last operation was
     * successful.
     */
    Q_INVOKABLE QString lastError();
    /**
     * Return the release version string for either a release, or a RnD snapshot
     */
    Q_INVOKABLE QString release(bool rnd=false);
    /**
     * Resolve a repository url
     * @return the repository URL on success, an empty string on error
     */
    QString repoUrl(QString repoName, bool rndRepo=false, QHash<QString, QString> repoParameters=QHash<QString, QString>());
    /**
     * Set the flavour used when resolving RND repositories
     */
    Q_INVOKABLE void setFlavour(QString flavour);
    /**
     * Set the release version string for either a release, or a RnD snapshot
     */
    Q_INVOKABLE void setRelease(QString release, bool rnd=false);
    /**
     * Unregister a device. This will clean all registration data from a device,
     * though will not touch the information on SSU server; the information there
     * has to be manually cleaned for a device we don't own anymore, but will be
     * overwritten next time the device gets registered
     */
    Q_INVOKABLE void unregister();
    /**
     * Return configuration settings regarding ssl verification
     * @retval true SSL verification must be used; that's the default if not configured
     * @retval false SSL verification should be disabled
     */
    Q_INVOKABLE bool useSslVerify();

  private:
    QString errorString;
    bool errorFlag;
    QNetworkAccessManager *manager;
    QSettings *settings, *repoSettings;
    bool registerDevice(QDomDocument *response);
    bool setCredentials(QDomDocument *response);
    bool verifyResponse(QDomDocument *response);

  private slots:
    void requestFinished(QNetworkReply *reply);
    /**
     * Set errorString returned by lastError to errorMessage, set
     * errorFlag returned by error() to true, and emit done()
     */
    void setError(QString errorMessage);

  public slots:
    /**
     * Attempt RND device registration, using @a username and @a password supplied
     * @param username Jolla username
     * @param password Jolla password
     *
     * When the operation has finished the done() signal will be sent. You can call
     * error() to check if an error occured, and use lastError() to retrieve the last
     * error message.
     */
    void sendRegistration(QString username, QString password);
    /**
     * Try to update the RND repository credentials. The device needs to be registered
     * for this to work. updateCredentials remembers the time of the last credentials
     * update, and skips updating if only little time has elapsed since the last update.
     * An update may be forced by setting @a force to true
     * @param force force credentials updating
     *
     * When the operation has finished the done() signal will be sent. You can call
     * error() to check if an error occured, and use lastError() to retrieve the last
     * error message.
     */
    void updateCredentials(bool force=false);

  signals:
    /**
     * Emitted after an asynchronous operation finished
     */
    void done();
    // we don't get notifications from settings -> this won't work over different instances (yet)
    void flavourChanged();
    void releaseChanged();
    void registrationStatusChanged();
    void credentialsChanged();

};

#endif