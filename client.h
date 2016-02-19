#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>
#include <QTcpSocket>

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

private slots:
// слот для обработки сигналов сокета
    void socketConnected();
// слот для об
    void socketReadyRead();
//***********************************
    void displayError(QAbstractSocket::SocketError socketError);
    void sessionOpened();

private:
    QPushButton *quitButton;
    QPushButton *connectToRobot;

    QPushButton *getImage;

    QPushButton *turnLeftEngine;
    QComboBox *leftEngineCombo;
    QLineEdit *leftEngineDegrees;

    QPushButton *turnRigtEngine;
    QComboBox *rightEngineCombo;
    QLineEdit *rightEngineDegrees;

    QLabel *directionToTurn;
    QLabel *degreesToTurn;
    QLabel *informMessage;

    QTcpSocket *tcpSocket;
    QComboBox *hostCombo;
    quint16 blockSize;

    QNetworkSession *networkSession;
};

#endif
