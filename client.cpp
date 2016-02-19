#include <QtWidgets>
#include <QtNetwork>

#include "client.h"

Client::Client(QWidget *parent)
:   QDialog(parent), networkSession(0)
{
    directionToTurn = new QLabel (tr("F/B"));
    degreesToTurn = new QLabel (tr("Degree"));

    leftEngineCombo = new QComboBox;
    leftEngineCombo->setEditable(true);
    leftEngineCombo->addItem(tr("F"));
    leftEngineCombo->addItem(tr("B"));

    rightEngineCombo = new QComboBox;
    rightEngineCombo->setEditable(true);
    rightEngineCombo->addItem(tr("F"));
    rightEngineCombo->addItem(tr("B"));

    leftEngineDegrees = new QLineEdit;
    leftEngineDegrees->setValidator(new QIntValidator(1, 65535, this));

    rightEngineDegrees = new QLineEdit;
    rightEngineDegrees->setValidator(new QIntValidator(1, 65535, this));

    informMessage = new QLabel(tr("Starting the program... Connect to robot."));

    quitButton = new QPushButton (tr("Exit"));
    connectToRobot = new QPushButton (tr("Connect to robot"));
    getImage = new QPushButton (tr("Get image from camera"));
    turnLeftEngine = new QPushButton(tr("Turn left engine"));
    turnRigtEngine = new QPushButton(tr("Turn right engine"));

//************************************** - Не менять
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // add non-localhost addresses
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (!ipAddressesList.at(i).isLoopback())
            hostCombo->addItem(ipAddressesList.at(i).toString());
    }
    // add localhost addresses
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i).isLoopback())
            hostCombo->addItem(ipAddressesList.at(i).toString());
    }
//************************************** - Не менять


// Создаем сокет
    tcpSocket = new QTcpSocket(this);
// Подключаем сигналы
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));

//*************************************

        QGridLayout *leftTopLayout = new QGridLayout;
        leftTopLayout->addWidget(directionToTurn, 0, 1);
        leftTopLayout->addWidget(degreesToTurn, 0, 2);

        leftTopLayout->addWidget(turnRigtEngine, 1, 0);
        leftTopLayout->addWidget(rightEngineCombo, 1, 1);
        leftTopLayout->addWidget(rightEngineDegrees, 1, 2);

        leftTopLayout->addWidget(turnLeftEngine, 2, 0);
        leftTopLayout->addWidget(leftEngineCombo, 2, 1);
        leftTopLayout->addWidget(rightEngineDegrees, 2, 2);

        leftTopLayout->addWidget(getImage, 3, 0, 3, 3, Qt::AlignCenter);

        setLayout(leftTopLayout);

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

void Client::socketConnected()
{

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
