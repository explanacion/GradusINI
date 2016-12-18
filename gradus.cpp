#include "gradus.h"
#include "ui_gradus.h"

Gradus::Gradus(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Gradus)
{
    ui->setupUi(this);
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
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerexpired()));
    timer->setInterval(period*1000);;
    timer->start();
    ui->statusBar->showMessage("Таймер запущен.");
    this->timerexpired();
    mayclose = false; 
    myfont = settings->value("settings/font","MS Shell Dlg 2").toString();
    fontsize = settings->value("settings/fontsize",ui->fontsizespinBox_2->value()).toInt();
    HTMLbefore = settings->value("settings/HTMLbefore","<div class=\"current-weather__thermometer current-weather__thermometer_type_now\">").toString();
    HTMLafter = settings->value("settings/HTMLafter","°C</div>").toString();
    ui->textEditBefore->setPlainText(HTMLbefore);
    ui->textEditAfter->setPlainText(HTMLafter);

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
    ui->lineEditOutput->setText(myfont);
    ui->spinBox->setDisabled(true);
    ui->lineEdit->setDisabled(true);
    ui->textEditBefore->setDisabled(true);
    ui->textEditAfter->setDisabled(true);
    ui->label_3->setDisabled(true);
    ui->lineEditOutput->setDisabled(true);
    ui->fontsizespinBox_2->setDisabled(true);
    ui->fontsizespinBox_2->setValue(fontsize);
}

void Gradus::showAlarmIcon()
{
    QIcon trayimage(":/new/prefix1/icon2_error.png");
    trayIcon->setIcon(trayimage);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
}

void Gradus::showTrayIcon()
{
    QIcon trayImage(":/new/prefix1/icon2.png");
    trayIcon->setIcon(trayImage);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
}

void Gradus::showTrayTemperature(QString value)
{
    QPixmap pixmap(24,24);
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    QString tmpvalue;
    if (value.length() < 2) {
        
        tmpvalue = value;
    }
    else {
        tmpvalue = value;
    }
    QFont font=painter.font();
    //font.setFamily("MS Shell Dlg 2");
    font.setFamily(myfont);
    font.setStyleStrategy(QFont::NoAntialias);
    font.setPointSize(fontsize);
    painter.setFont(font);
    painter.drawText(pixmap.rect(),Qt::AlignLeft | Qt::AlignVCenter,tmpvalue);


    trayIcon->setIcon(pixmap);
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
    delete ui;
}



void Gradus::on_pushButton_clicked()
{
    // check font
    QFontDatabase fd;
    if (!fd.hasFamily(ui->lineEditOutput->text().trimmed()))
    {
        ui->statusBar->showMessage("Указанный в настройках шрифт не найден. Вместо него будет использован один из стандартных");
        ui->lineEditOutput->setStyleSheet("color: red;");
    }
    else {
        ui->statusBar->showMessage("Шрифт найден");
        ui->lineEditOutput->setStyleSheet("color: gray;");
    }

    ui->pushButton_2->setEnabled(true);
    ui->statusBar->clearMessage();
    if (timer->isActive())
        timer->stop();

    period = ui->spinBox->value();
    timer->start(1000*period);
    ui->pushButton->setDisabled(true);
    
    settings->setValue("settings/period",period);

    settings->setValue("settings/url",ui->lineEdit->text());
    
    settings->setValue("settings/HTMLbefore",ui->textEditBefore->toPlainText().trimmed());
    settings->setValue("settings/HTMLafter",ui->textEditAfter->toPlainText().trimmed());
    settings->setValue("settings/font",ui->lineEditOutput->text());
    settings->setValue("settings/fontsize",QString::number(ui->fontsizespinBox_2->value()));
    myfont = ui->lineEditOutput->text();
    fontsize = ui->fontsizespinBox_2->value();
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
    ui->label_3->setDisabled(true);
    ui->lineEditOutput->setDisabled(true);
    ui->lineEdit->setDisabled(true);
    ui->fontsizespinBox_2->setDisabled(true);
}

float CeltoFahr(float cels)
{
    return cels*9/5+32;
}

void Gradus::replyFinished(QNetworkReply *reply) {
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
//            if (res.length() > 3)
//            {
//                ui->statusBar->showMessage("Длина выводимого сообщения превышает 3 символа.");
//                isAlarm = true;
//                showAlarmIcon();
//                return;
//            }
            showTrayTemperature(res);

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
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
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
    ui->lineEditOutput->setStyleSheet("color: black;");
    ui->statusBar->showMessage("Таймер остановлен. Программа в режиме ожидания");
    ui->spinBox->setEnabled(true);
    ui->textEditBefore->setEnabled(true);
    ui->textEditAfter->setEnabled(true);
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setDisabled(true);
    ui->label_3->setEnabled(true);
    ui->lineEditOutput->setEnabled(true);
    ui->lineEdit->setEnabled(true);
    ui->fontsizespinBox_2->setEnabled(true);
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
