#include "gradus.h"
#include "ui_gradus.h"
#include <QToolTip>

bool strToBool(QString strr)
{
    if (strr == "1")
        return true;
    else
        return false;
}

Gradus::Gradus(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Gradus)
{
    ui->setupUi(this);

    trayAlarmimage = new QIcon(":/new/prefix1/icon2_error.png");
    trayImage = new QIcon(":/new/prefix1/icon2.png");
    whitepixmap = new QPixmap(16,16);

    manager = new QNetworkAccessManager(this);
    trayIcon = new QSystemTrayIcon(this);
    isAlarm = false;
    settings = new QSettings("settings.conf",QSettings::IniFormat);
    int periods = settings->value("settings/period",300).toInt();

    ui->spinBox->setValue(periods);
    QString lineEditUrl = settings->value("settings/url","https://yandex.ru/pogoda/krasnoyarsk").toString();
    ui->lineEdit->setText(lineEditUrl);
    
    period = ui->spinBox->value();
    url = ui->lineEdit->text();
    this->setTrayIconActions();
    if (settings->isWritable())
    {
        this->showTrayIcon();
    }
    else {
        this->showAlarmIcon();
        isAlarm = true;
        ui->statusBar->showMessage("Файл настроек доступен только для чтения.");
    }
    mayclose = false;
    myfont.fromString(settings->value("settings/font","Arial,8,-1,5,75,0,0,0,0,0").toString());
    trayLength = settings->value("settings/traylength","3").toInt();
    HTMLbefore = settings->value("settings/HTMLbefore","<div class=\"current-weather__thermometer current-weather__thermometer_type_now\">").toString();
    HTMLafter = settings->value("settings/HTMLafter","°C</div>").toString();
    ui->textEditBefore->setPlainText(HTMLbefore);
    ui->textEditAfter->setPlainText(HTMLafter);
    aliasing = strToBool(settings->value("settings/aliasing","0").toString());

    if (aliasing)
    {
        ui->AliasingcheckBox->setChecked(true);
    }
    bgcolor = QColor(settings->value("settings/bgcolor","#000000").toString());
    txtcolor = QColor(settings->value("settings/textcolor","#00ff00").toString());

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

    ui->spinBox->setDisabled(true);
    ui->lineEdit->setDisabled(true);
    ui->textEditBefore->setDisabled(true);
    ui->textEditAfter->setDisabled(true);
    ui->FontpushButton_3->setDisabled(true);
    ui->label_7->setDisabled(true);
    ui->trayLengthspinBox->setDisabled(true);
    ui->AliasingcheckBox->setDisabled(true);
    ui->BackcolortoolButton->setDisabled(true);
    ui->TextColortoolButton_2->setDisabled(true);

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerexpired()));
    timer->setInterval(period*1000);;
    timer->start();
    ui->statusBar->showMessage("Таймер запущен.");
    this->timerexpired();

    this->trayIcon->setToolTip("TrayIconInformer v1.7 " + url);
}

void Gradus::showAlarmIcon()
{
    trayIcon->setIcon(*trayAlarmimage);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
}

void Gradus::showTrayIcon()
{
    trayIcon->setIcon(*trayImage);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
}

void Gradus::showTrayTemperature(QString value)
{ 
    whitepixmap->fill(bgcolor);
    QPainter painter;
    painter.begin(whitepixmap);

    if (!aliasing) {
        myfont.setStyleStrategy(QFont::NoAntialias);
        painter.setRenderHint(QPainter::TextAntialiasing, true);
    }
    else {
        myfont.setStyleStrategy(QFont::PreferAntialias);
    }

    painter.setFont(myfont);
    painter.setPen(txtcolor);
    painter.drawText(whitepixmap->rect(),Qt::AlignCenter | Qt::AlignVCenter,value);
    trayIcon->setIcon(*whitepixmap);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
}

void Gradus::trayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
            if (this->isHidden())
            {
                this->showNormal();
                this->activateWindow();
            }
            else
            {
                this->hide();
            }
            break;
        default:
            break;
    }
}

void Gradus::setTrayIconActions()
{
    minimizeAction = new QAction("Свернуть", this);
    restoreAction = new QAction("Восстановить", this);
    quitAction = new QAction("Выход", this);

    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(toquit()));
    
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addAction(quitAction);
}

void Gradus::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);
    if (event -> type() == QEvent::WindowStateChange)
    {
        if (isMinimized())
        {
            this -> hide();
        }
    }
}

Gradus::~Gradus()
{
    trayIcon->hide();
    delete trayAlarmimage;
    delete trayImage;
    delete whitepixmap;
    delete settings;
    delete ui;
}


QString boolToStr(bool inp)
{
    if (inp)
        return "1";
    else
        return "0";
}

void Gradus::on_pushButton_clicked()
{
    showTrayIcon();
    ui->pushButton_2->setEnabled(true);
    ui->statusBar->clearMessage();
    if (timer->isActive())
        timer->stop();

    period = ui->spinBox->value();
    timer->start(1000*period);
    ui->pushButton->setDisabled(true);
    
    settings->setValue("settings/period",period);

    settings->setValue("settings/url",ui->lineEdit->text());
    settings->setValue("settings/traylength",QString::number(ui->trayLengthspinBox->value()));

    settings->setValue("settings/aliasing",boolToStr(ui->AliasingcheckBox->isChecked()));
    settings->setValue("settings/bgcolor",bgcolor.name(QColor::HexRgb));
    settings->setValue("settings/textcolor",txtcolor.name(QColor::HexRgb));
    settings->setValue("settings/HTMLbefore",ui->textEditBefore->toPlainText().trimmed());
    settings->setValue("settings/HTMLafter",ui->textEditAfter->toPlainText().trimmed());
    settings->setValue("settings/font",myfont.toString());
    settings->setValue("settings/traylength",QString::number(ui->trayLengthspinBox->value()));

    trayLength = ui->trayLengthspinBox->value();
    aliasing = ui->AliasingcheckBox->isChecked();
    url = ui->lineEdit->text();
    HTMLbefore = ui->textEditBefore->toPlainText().trimmed();
    HTMLafter = ui->textEditAfter->toPlainText().trimmed();
    if (isAlarm) {
        showTrayIcon();
    }
    readdata(QUrl(url));

    ui->spinBox->setDisabled(true);
    ui->lineEdit->setDisabled(true);
    ui->textEditBefore->setDisabled(true);
    ui->textEditAfter->setDisabled(true);
    ui->FontpushButton_3->setDisabled(true);
    ui->lineEdit->setDisabled(true);
    ui->FontpushButton_3->setDisabled(true);
    ui->label_7->setDisabled(true);
    ui->trayLengthspinBox->setDisabled(true);
    ui->AliasingcheckBox->setDisabled(true);
    ui->BackcolortoolButton->setDisabled(true);
    ui->TextColortoolButton_2->setDisabled(true);
}

float CeltoFahr(float cels)
{
    return cels*9/5+32;
}

void Gradus::replyFinished(QNetworkReply *reply) {
    reply->deleteLater();
    if (reply->error() == QNetworkReply::HostNotFoundError) {
        ui->statusBar->showMessage("Адрес неверный. Подключение невозможно.");
        isAlarm=true;
        showAlarmIcon();
        return;
    }
    if (reply->error() == QNetworkReply::ContentNotFoundError) {
        ui->statusBar->showMessage("Страница не найдена.");
        isAlarm=true;
        showAlarmIcon();
        return;
    }
    if (reply->error() == QNetworkReply::NoError or reply->error() == QNetworkReply::UnknownNetworkError)
    {
        QString statuscode = QString::number(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
        ui->statusBar->showMessage("HTTP ответ - " + statuscode + " " + reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString());
        if (statuscode == "200")
        {
            if (isAlarm) {
                showTrayIcon();
            }
            isAlarm = false;
            
            ui->statusBar->clearMessage();
            ui->statusBar->showMessage("Соединение было успешно установлено.");

            QByteArray rbytes = reply->readAll(); 
            QString temp(rbytes);
            int pos1 = temp.indexOf(HTMLbefore);

            if (pos1 == -1) {
                showAlarmIcon();
                ui->statusBar->showMessage("HTML-код до значения температуры не найден.");
                return;
            }

            int len1 = HTMLbefore.length();
            temp = temp.mid(pos1+len1); 
            int pos2 = temp.indexOf(HTMLafter);
            if (pos2 == -1) {
                showAlarmIcon();
                ui->statusBar->showMessage("HTML-код после значения температуры не найден.");
                return;
            }
            QString res = temp.mid(0,pos2);
            res=res.trimmed();
            showTrayTemperature(res.mid(0,trayLength));

            QDateTime date = QDateTime::currentDateTime();
            QString timestamp = date.toString("yyyy-MM-dd-hh.mm.ss");
            ui->statusBar->showMessage("Время: " + timestamp + " Значение: " + res);
        }
    }
    else {
        ui->statusBar->clearMessage();
        ui->statusBar->showMessage(reply->errorString());
        isAlarm = true;
        showAlarmIcon();
    }
}

bool isConnectedToNetwork()
{
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    bool result = false;

    for (int i = 0; i < ifaces.count(); i++)
    {
        QNetworkInterface iface = ifaces.at(i);
        if ( iface.flags().testFlag(QNetworkInterface::IsUp)
             && !iface.flags().testFlag(QNetworkInterface::IsLoopBack) )
        {

#ifdef DEBUG
            
            qDebug() << "name:" << iface.name() << endl
                    << "ip addresses:" << endl
                    << "mac:" << iface.hardwareAddress() << endl;
#endif

            
            for (int j=0; j<iface.addressEntries().count(); j++)
            {
#ifdef DEBUG
                qDebug() << iface.addressEntries().at(j).ip().toString()
                        << " / " << iface.addressEntries().at(j).netmask().toString() << endl;
#endif

                if (result == false)
                    result = true;
            }
        }
    }
    return result;
}


void Gradus::readdata(QUrl url) {
    if (!isConnectedToNetwork())
    {
        this->showAlarmIcon();
        ui->statusBar->showMessage("Похоже, нет подключения к сети.");
        isAlarm = true;
        
        return;
    }
    if (url.toString().contains("https")) {
        QSslConfiguration config = QSslConfiguration::defaultConfiguration();
        config.setProtocol(QSsl::TlsV1_2);
        QNetworkRequest request;
        request.setSslConfiguration(config);
        request.setUrl(url);
        manager->get(request);
        return;
    }
    manager->get(QNetworkRequest(url));
}

void Gradus::timerexpired()
{
    readdata(QUrl(url));
    
    if (!timer->isActive())
    {
        timer->start(1000*ui->spinBox->value());
    }
    else {
        if (ui->pushButton->isEnabled())
            ui->pushButton->setDisabled(true);
    }
}

void Gradus::on_pushButton_2_clicked()
{
    timer->stop();
    showAlarmIcon();
    ui->statusBar->showMessage("Таймер остановлен. Программа в режиме ожидания");
    ui->spinBox->setEnabled(true);
    ui->textEditBefore->setEnabled(true);
    ui->textEditAfter->setEnabled(true);
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setDisabled(true);
    ui->FontpushButton_3->setEnabled(true);
    ui->lineEdit->setEnabled(true);
    ui->label_7->setEnabled(true);
    ui->trayLengthspinBox->setEnabled(true);
    ui->AliasingcheckBox->setEnabled(true);
    ui->BackcolortoolButton->setEnabled(true);
    ui->TextColortoolButton_2->setEnabled(true);
    ui->lineEdit->setFocus();

}

void Gradus::closeEvent(QCloseEvent *ev)
{
    if (!mayclose) {
        ev->ignore();
        this->hide();
    }
    else {
        ev->accept();
    }
}

void Gradus::toquit()
{
    mayclose=true;
    this->close();
}

void Gradus::on_BackcolortoolButton_clicked()
{
    QColor tmpcolor = QColorDialog::getColor(bgcolor);
    if (tmpcolor.isValid()) {
        bgcolor = tmpcolor;
    }
}

void Gradus::on_TextColortoolButton_2_clicked()
{
    QColor tmpcolor = QColorDialog::getColor(txtcolor);
    if (tmpcolor.isValid())
    {
        txtcolor = tmpcolor;
    }
}

void Gradus::on_FontpushButton_3_clicked()
{
    bool bOK;
    QFont &oldfont = myfont;
    QFontDialog fd;
    fd.setCurrentFont(oldfont);
    myfont = fd.getFont(&bOK,oldfont);

}
