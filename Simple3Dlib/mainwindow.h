#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <stdint.h>
#include <QFile>
#include <QSettings>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QPixmap* pix;
    QPixmap* texture;
    QPixmap* texture_rgb;
public slots:
    void refresh();
    void keyPressEvent( QKeyEvent* event );

private slots:
    void on_actionZoom_X2_triggered();

    void on_actionZoom_X4_triggered();

    void on_actionZoom_X8_triggered();

    void on_actionZoom_X1_triggered();

    void on_actionExport_texture_triggered();

    void on_actionConvert_object_triggered();

private:
    Ui::MainWindow *ui;
    QTimer timer_refesh;
    uint32_t u32_current_zoom;
    void update_zoom(uint32_t new_zoom);
    void export_8b( QFile* fileout, QImage& img );
    void convert2Obj(); // convert WaveFront object to H
    QSettings* settings;                    // app settings : store default path ( located into Windows register base )

};

#endif // MAINWINDOW_H

