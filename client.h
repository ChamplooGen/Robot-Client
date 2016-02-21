#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>
#include <QTcpSocket>

class QPushButton;
class QTcpSocket;
class QNetworkSession;
class QLabel;
class QComboBox;
class QLineEdit;

class Client : public QDialog
{
    Q_OBJECT

public:
    Client(QWidget *parent = 0);

    struct Command
    {
        char keyWord;
        quint16 length;
        QString data;
    };

private slots:
// слот для подключения к роботу - можно упростить, убрав hostCombo и portLineEdit (сделать по default)
    void connectToRobot();

// слот для проверки того, что подключились к роботу - в дальнейшем можно убрать,
// перевалив его функции на socketReadyRead();
    void socketConnected();

// слот для обработки информации из сокета
    void socketReadyRead();

// слот для пересылки команд

    void sendCommand(Command command);
    void buildCommand();




//***********************************
    void displayError(QAbstractSocket::SocketError socketError);
    void sessionOpened();

signals:
    void sendCommandSignal(Command command);

private:
// Можно упростить
    QLabel *hostLabel;
    QLabel *portLabel;
    QComboBox *hostCombo;
    QLineEdit *portLineEdit;
//*****************************
    QPushButton *quitButton;
    QPushButton *connectToRobotButton;

    QPushButton *getImage;

    QPushButton *turnLeftEngineButton;
    QComboBox *leftEngineCombo;
    QLineEdit *leftEngineDegrees;

    QPushButton *turnRigtEngineButton;
    QComboBox *rightEngineCombo;
    QLineEdit *rightEngineDegrees;

    QLabel *directionToTurn;
    QLabel *degreesToTurn;
    QLabel *informMessage;
    QLabel *videoArea;

    QTcpSocket *tcpSocket;
    quint16 blockSize;

    QNetworkSession *networkSession;



};

#endif
