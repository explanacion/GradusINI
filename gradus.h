#ifndef GRADUS_H
#define GRADUS_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QMenu>
#include <QIcon>
#include <QString>
#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QTextStream>
#include <QSettings>
#include <QPainter>
#include <QSsl>
#include <QSslConfiguration>
#include <QCloseEvent>
#include <QEventLoop>
#include <QColorDialog>
#include <QFontDialog>

namespace Ui {
class Gradus;
}

class Gradus : public QMainWindow
{
    Q_OBJECT

public:
    explicit Gradus(QWidget *parent = 0);
    QTimer *timer;
    QString url;
    unsigned int period;
    QSettings *settings;
    bool isAlarm;
    QString HTMLbefore;
    QString HTMLafter;
    QFont myfont;
    bool mayclose;
    int fontstyle;
    bool aliasing;
    int trayLength;
    QColor bgcolor;
    QColor txtcolor;
    QNetworkAccessManager *manager;
    ~Gradus();
public slots:
    void timerexpired(); 
    void showAlarmIcon();
    void toquit();

private:
    void closeEvent(QCloseEvent *ev);
    QIcon *trayAlarmimage;
    QIcon *trayImage;
    QPixmap *whitepixmap;

    QMenu *trayIconMenu;
    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    QSystemTrayIcon *trayIcon;
    Ui::Gradus *ui;
private slots:
    void changeEvent(QEvent*);
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void setTrayIconActions();
    void showTrayIcon();
    void showTrayTemperature(QString value);
    void on_pushButton_clicked();
    void readdata(QUrl url);
    
    void replyFinished(QNetworkReply *reply);
    void on_pushButton_2_clicked();
    void on_BackcolortoolButton_clicked();
    void on_TextColortoolButton_2_clicked();
    void on_FontpushButton_3_clicked();
};

#endif 
