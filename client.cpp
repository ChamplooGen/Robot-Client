#include <QtWidgets>
#include <QtNetwork>

#include "client.h"

Client::Client(QWidget *parent)
:   QDialog(parent), networkSession(0)
{
    videoArea = new QLabel(tr("Here will be video from robot"));

    directionToTurn = new QLabel (tr("F/B"));
    degreesToTurn = new QLabel (tr("Degree"));

    leftEngineCombo = new QComboBox;
    leftEngineCombo->setEditable(false);
    leftEngineCombo->addItem(tr("F"));
    leftEngineCombo->addItem(tr("B"));

    rightEngineCombo = new QComboBox;
    rightEngineCombo->setEditable(false);
    rightEngineCombo->addItem(tr("F"));
    rightEngineCombo->addItem(tr("B"));

    leftEngineDegrees = new QLineEdit;
    leftEngineDegrees->setValidator(new QIntValidator(1, 65535, this));
    leftEngineDegrees->setFixedSize(80, 30);

    rightEngineDegrees = new QLineEdit;
    rightEngineDegrees->setValidator(new QIntValidator(1, 65535, this));
    rightEngineDegrees->setFixedSize(80, 30);

    informMessage = new QLabel(tr("Starting the program... Connect to robot."));

    quitButton = new QPushButton (tr("Exit"));
    connectToRobotButton = new QPushButton (tr("Connect to robot"));
    getImage = new QPushButton (tr("Get image from camera"));
    turnLeftEngineButton = new QPushButton(tr("Turn left engine"));
    turnRightEngineButton = new QPushButton(tr("Turn right engine"));

    // Спорная часть виджета
    hostLabel = new QLabel(tr("Server name:"));
    portLabel = new QLabel(tr("Server port:"));

    hostCombo = new QLineEdit;
    //hostCombo->setEditable(true);

    portLineEdit = new QLineEdit;
    portLineEdit->setValidator(new QIntValidator(1, 65535, this));
//************************************** - Не менять
//    // find out name of this machine
//    QString name = QHostInfo::localHostName();
//    if (!name.isEmpty()) {
//        hostCombo->addItem(name);
//        QString domain = QHostInfo::localDomainName();
//        if (!domain.isEmpty())
//            hostCombo->addItem(name + QChar('.') + domain);
//    }
//    if (name != QString("localhost"))
//        hostCombo->addItem(QString("localhost"));
//    // find out IP addresses of this machine
//    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
//    // add non-localhost addresses
//    for (int i = 0; i < ipAddressesList.size(); ++i) {
//        if (!ipAddressesList.at(i).isLoopback())
//            hostCombo->addItem(ipAddressesList.at(i).toString());
//    }
//    // add localhost addresses
//    for (int i = 0; i < ipAddressesList.size(); ++i) {
//        if (ipAddressesList.at(i).isLoopback())
//            hostCombo->addItem(ipAddressesList.at(i).toString());
//    }
//************************************** - Не менять

// Создаем сокет

    tcpSocket = new QTcpSocket(this);

// Подключаем сигналы
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(connectToRobotButton, SIGNAL(clicked()), this, SLOT(connectToRobot()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));


//    connect(turnLeftEngineButton, SIGNAL(clicked()), this, SLOT(buildLeftEngineCommand()));
//    connect(turnRightEngineButton, SIGNAL(clicked()), this, SLOT(buildRightEngineCommand()));
    connect(getImage, SIGNAL(clicked()), this, SLOT(buildCamCmd()));
    connect(this, SIGNAL(sendCommandSignal(Command)), this, SLOT(sendCommand(Command)));

//*************************************
// Левая верхняя часть layout'a
        QGridLayout *leftTopLayout = new QGridLayout;
        leftTopLayout->addWidget(directionToTurn, 0, 1);
        leftTopLayout->addWidget(degreesToTurn, 0, 2);

        leftTopLayout->addWidget(turnRightEngineButton, 1, 0);
        leftTopLayout->addWidget(rightEngineCombo, 1, 1);
        leftTopLayout->addWidget(rightEngineDegrees, 1, 2);

        leftTopLayout->addWidget(turnLeftEngineButton, 2, 0);
        leftTopLayout->addWidget(leftEngineCombo, 2, 1);
        leftTopLayout->addWidget(leftEngineDegrees, 2, 2);

        leftTopLayout->addWidget(getImage, 3, 0, 1, 3, Qt::AlignCenter);

// Правая нижняя часть layout'a
        QGridLayout *rightBottomLayout = new QGridLayout;
        rightBottomLayout->addWidget(hostLabel, 0, 0);
        rightBottomLayout->addWidget(portLabel, 1, 0);
        rightBottomLayout->addWidget(hostCombo, 0, 1);
        rightBottomLayout->addWidget(portLineEdit, 1, 1);
        rightBottomLayout->addWidget(connectToRobotButton, 2, 0);
        rightBottomLayout->addWidget(quitButton, 2, 1);
//Основная часть layout'a
        QGridLayout *mainLayout = new QGridLayout;
        mainLayout->addLayout(leftTopLayout, 0, 0);
        mainLayout->addWidget(videoArea, 0, 1, 1, 1, Qt::AlignCenter);
        mainLayout->addWidget(informMessage, 1, 0, 1, 1, Qt::AlignCenter);
        mainLayout->addLayout(rightBottomLayout, 1, 1);

        setLayout(mainLayout);
        resize(550, 300);

    setWindowTitle(tr("Robot Client"));


//************************************************************************* - Не менять
    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));

        //statusLabel->setText(tr("Opening network session."));
        networkSession->open();
    }
    //************************************************************************* - Не менять
}

void Client::keyPressEvent(QKeyEvent *event)
{
    int key = event->key(); // целочисленный код клавиши
    switch (key)
    {
        case Qt::Key_Left:
            buildMoveLeftCmd();
        break;

        case Qt::Key_Right:
            buildMoveRightCmd();
        break;

        case Qt::Key_Up:
            buildMoveForfardCmd();
        break;

        case Qt::Key_Down:
            buildMoveBackCmd();
        break;

        case Qt::Key_Space:
            buildMoveStopCmd();
        break;

        case Qt::Key_I:
            buildCamCmd();
        break;

        case Qt::Key_F1:
            this->setFocus();
        break;
//        default:
//        QWidget::event(event);
//        break;
    }

}

//void Client::keyReleaseEvent(QKeyEvent *event)
//{
//    int key = event->key(); // целочисленный код клавиши
//    switch (key)
//    {
//        case Qt::Key_Left:
//            buildMoveStopCmd();
//        break;

//        case Qt::Key_Right:
//            buildMoveStopCmd();
//        break;

//        case Qt::Key_Up:
//            buildMoveStopCmd();
//        break;

//        case Qt::Key_Down:
//            buildMoveStopCmd();
//        break;
//    }
//}


void Client::connectToRobot()
{
    connectToRobotButton->setEnabled(false);
    blockSize = 0;
    tcpSocket->abort();
    tcpSocket->connectToHost(hostCombo->text(), portLineEdit->text().toInt());
    this->setFocus();
}

void Client::socketConnected()
{
    informMessage->setText(tr("You were sucessfully connected.\n Congratulations!"));
}

void Client::socketReadyRead()  // обработка данных от сервера
{
    QDataStream in(tcpSocket);

    if (blockSize == 0) // только начинаме чтение данных
    {
        if (tcpSocket->bytesAvailable() < (int)sizeof(quint16))
            return;
        in >> blockSize;    // считали размер блока данных
    }

    if (tcpSocket->bytesAvailable() < blockSize)
        return; // ждем, пока данные прийдут полностью;
    else blockSize = 0;

    QString message;
    in >> message;
    //informMessage->setText(message);
}

void Client::buildMoveForfardCmd()
{
    Command command;
    command.keyWord = 'M';      // Move
    command.data.append('R');   // Run
    command.data.append('F');   // Forward

    emit sendCommandSignal(command);
}

void Client::buildMoveBackCmd()
{
    Command command;
    command.keyWord = 'M';
    command.data.append('R');
    command.data.append('B');

    emit sendCommandSignal(command);
}

void Client::buildMoveLeftCmd()
{
    Command command;
    command.keyWord = 'M';
    command.data.append('R');
    command.data.append('L');

    emit sendCommandSignal(command);
}

void Client::buildMoveRightCmd()
{
    Command command;
    command.keyWord = 'M';
    command.data.append('R');
    command.data.append('R');

    emit sendCommandSignal(command);
}

void Client::buildMoveStopCmd()
{
    Command command;
    command.keyWord = 'M';
    command.data.append('S');   // Stop

    emit sendCommandSignal(command);
}

void Client::buildCamCmd()
{
    Command command;
    command.keyWord = 'I';

    emit sendCommandSignal(command);
}

void Client::sendCommand(Command command)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    out << quint16(0);
    qDebug() << " 1)Data: " << block;
    out << QChar(command.keyWord);
    qDebug() << " 2)Data: " << block;
    out << command.data;
    qDebug() << " 3)Data: " << block;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));

//    qDebug() << " 4)Data: " << block;
//    qDebug() << " Data is empty? - " << block.isEmpty();


//    qDebug() << " Socket is writable? " << tcpSocket->isWritable();
//    qDebug() << " Amount of available bytes in socket: " << tcpSocket->bytesAvailable();

    tcpSocket->write(block);
    qDebug() << " Command was sent to robot";
    informMessage->setText(" Your command was sent to robot.\n" + QString(command.keyWord) + " "+ command.data);
}

void Client::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(tcpSocket->errorString()));
    }
}

void Client::sessionOpened()
{
    // Save the used configuration
    QNetworkConfiguration config = networkSession->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice)
        id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
    else
        id = config.identifier();

    QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();

}
