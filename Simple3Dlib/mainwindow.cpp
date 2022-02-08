#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "modelViewer.hpp"

#include <QPainter>
#include <QDebug>
#include <QKeyEvent>
#include <QFileInfo>
#include <QFileDialog>
Gb gb;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    settings = 0;
    settings = new QSettings( "Gamebuino", "OBJCOnverter" );
    timer_refesh.setInterval(40);
    connect(&timer_refesh, SIGNAL(timeout()), this, SLOT(refresh()));
    pix = new QPixmap( SCREEN_WIDTH, SCREEN_HEIGHT);
    pix->fill( Qt::blue );
    texture = new QPixmap( TEXTURE_W, TEXTURE_H );
    texture_rgb = new QPixmap( 256, 256);
    texture->fill( Qt::blue );
    timer_refesh.start();
    s3l_setup();
    update_zoom(2);
}

MainWindow::~MainWindow()
{
    if ( settings )
        delete settings;
    delete ui;
}

void MainWindow::export_8b( QFile* fileout, QImage& img )
{
    QString str_pict_name = QFileInfo(fileout->fileName()).baseName();
    str_pict_name.replace( " ", "_" );
    qDebug() << "generation object " << str_pict_name;
    // palette
    fileout->write( "//palette : \r\n"  );
    fileout->write( QString().sprintf("const uint16_t %s_rgb565_palette[%d] = { ",
                                      str_pict_name.toUtf8().data(),
                                      256).toUtf8().data()  );
    for ( int i = 0 ; i < 256 ; i++ )
    {
        QRgb c = img.color(i);
        if ( i >= img.colorCount() )
                c = 0;
        qDebug() << " image color " << i << " :  R = " << qRed(c) << " G = " << qGreen(c) << " B = " << qBlue(c) ;
        uint16_t u16_rgb565 = rgb888Torgb565( qRed(c), qGreen(c), qBlue(c) );
        qDebug() << " image color " << i << " :  565 = " << u16_rgb565 ;
        if(i)
            fileout->write(", ");
        fileout->write( QString().sprintf("0x%04X", u16_rgb565 ).toUtf8().data()  );

    }
    fileout->write( " };\r\n"  );

    // bitmap : w * h x uint8_t
    fileout->write( "//databit : \r\n"  );
    fileout->write( QString().sprintf( "#define %s_PICT_WIDTH %d\r\n", str_pict_name.toUtf8().data(), img.width() ).toUtf8().data() );
    fileout->write( QString().sprintf( "#define %s_PICT_HEIGHT %d\r\n", str_pict_name.toUtf8().data(), img.height() ).toUtf8().data() );
    fileout->write( QString().sprintf( "const uint8_t %s_data[%s_PICT_WIDTH*%s_PICT_HEIGHT] = { ", str_pict_name.toUtf8().data(), str_pict_name.toUtf8().data(), str_pict_name.toUtf8().data()  ).toUtf8().data()  );
    uint8_t u16_idx;
    int iFirst = 0;
    for ( uint32_t u32_y = 0 ; u32_y < img.height() ; u32_y++ )
    {
        fileout->write( "\r\n\t"  );
        for ( uint32_t u32_x = 0 ; u32_x < img.width() ; u32_x++ )
        {
            u16_idx = img.pixelIndex(u32_x, u32_y );
            if ( iFirst++ )
                fileout->write( ", "  );
            fileout->write( QString().sprintf("0x%02X", u16_idx ).toUtf8().data()  );
        }
    }
    fileout->write( "\r\n};\r\n"  );
}

void MainWindow::refresh()
{
    s3l_loop();
    QPainter painter(pix);
    for ( unsigned int y = 0; y < SCREEN_HEIGHT; y++ )
    {
        for ( unsigned int x = 0; x < SCREEN_WIDTH; x++ )
        {
          Gamebuino_Meta::Color col = gb.display.vscreen[ x+y*SCREEN_WIDTH ];
          painter.setPen( rgb565Torgb888( swap16( col ) ) );
          painter.drawPoint(x, y);
        }
    }
    QPixmap resize = pix->scaled( SCREEN_WIDTH*u32_current_zoom, SCREEN_HEIGHT*u32_current_zoom);

    ui->label_vscreen->setPixmap( resize );
}


void MainWindow::keyPressEvent( QKeyEvent* event )
{
    switch ( event->key() ) {
        case Qt::Key_Up:    gb.buttons.u32_buttons |= BUTTON_UP;    break;
        case Qt::Key_Down:  gb.buttons.u32_buttons |= BUTTON_DOWN;    break;
        case Qt::Key_Left:    gb.buttons.u32_buttons |= BUTTON_LEFT;    break;
        case Qt::Key_Right:    gb.buttons.u32_buttons |= BUTTON_RIGHT;    break;
        case Qt::Key_1:    gb.buttons.u32_buttons |= BUTTON_B;    break; // change object
        case Qt::Key_PageUp:    gb.buttons.u32_buttons |= BUTTON_A | BUTTON_UP;    break; // zoom in
        case Qt::Key_PageDown:    gb.buttons.u32_buttons |= BUTTON_A | BUTTON_DOWN;    break; // zoom in
    }
}

void MainWindow::update_zoom(uint32_t new_zoom)
{
    u32_current_zoom = new_zoom;
    QRect rc = ui->label_vscreen->geometry();
    rc.setTop(10);
    rc.setLeft(10);
    rc.setHeight( SCREEN_HEIGHT * u32_current_zoom );
    rc.setWidth( SCREEN_WIDTH * u32_current_zoom );
    ui->label_vscreen->setGeometry( rc );
}

void MainWindow::on_actionZoom_X2_triggered()
{
    update_zoom(2);
}

void MainWindow::on_actionZoom_X4_triggered()
{
    update_zoom(4);
}

void MainWindow::on_actionZoom_X8_triggered()
{
    update_zoom(8);
}

void MainWindow::on_actionZoom_X1_triggered()
{
    update_zoom(1);
}

void MainWindow::on_actionExport_texture_triggered()
{
    QString str_file_folder = QDir::currentPath();
    if ( settings )
        str_file_folder = settings->value( "last_texture", "" ).toString();

    QString strFileName = QFileDialog::getOpenFileName( this,
                        "Select existing file", str_file_folder, "Picture 256 colors (*.bmp)"  );


    if ( !strFileName.length() ) // abort operation
        return ;

    if ( settings )
           settings->setValue( "last_texture", strFileName );

    QImage img;
    img.load( strFileName );

    strFileName.append(".h");

    QFile* fileout = new QFile(strFileName);
    fileout->open(QFile::WriteOnly);
    export_8b( fileout, img );
    fileout->close();
}




void MainWindow::convert2Obj()
{
    QString str_file_folder = QDir::currentPath();

    if ( settings )
        str_file_folder = settings->value( "last_obj", "" ).toString();
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  "obj to convert",
                                                  str_file_folder,
                                                  "*.obj");

  QFile file_in(fileName);

  if (!file_in.open(QIODevice::ReadOnly | QIODevice::Text))
    return;

  if ( settings )
         settings->setValue( "last_obj", fileName );

  QList <float>vertices_xyz;
  QList <float>textcord_uv;
  QList <int>textures_uv_index;
  QList <int>faces_vertices_index;
  QStringList sl, sl2;
  while(!file_in.atEnd())
  {
    QString line = QString(file_in.readLine().data());
    sl = line.split(" ");

    if(sl.size() >= 4 && sl[0] == "v")
    {
      vertices_xyz.append( atof(sl[1].toStdString().data()) );
      vertices_xyz.append( atof(sl[2].toStdString().data()) );
      vertices_xyz.append( atof(sl[3].toStdString().data()) );
    }
    else if(sl.size() == 4 && sl[0] == "f")
    {
      sl2 = sl[1].split("/");
      if ( sl2.size() < 2 ) sl2.append("1"); // add fake texture index
      faces_vertices_index.append( atoi(sl2[0].toStdString().data())-1 );
      textures_uv_index.append( atoi(sl2[1].toStdString().data())-1 );

      sl2 = sl[2].split("/");
      if ( sl2.size() < 2 ) sl2.append("1");    // add fake texture index
      faces_vertices_index.append( atoi(sl2[0].toStdString().data())-1 );
      textures_uv_index.append( atoi(sl2[1].toStdString().data())-1 );

      sl2 = sl[3].split("/");
      if ( sl2.size() < 2 ) sl2.append("1");    // add fake texture index
      faces_vertices_index.append( atoi(sl2[0].toStdString().data())-1 );
      textures_uv_index.append( atoi(sl2[1].toStdString().data())-1 );
    }
    else if(sl.size() == 3 && sl[0] == "vt" )
    {
        textcord_uv.append( atof(sl[1].toStdString().data()) );
        textcord_uv.append( atof(sl[2].toStdString().data()) );
    }
  }

  file_in.close();
  float fmax[3] = {-1e6, -1e6, -1e6};
  float fmin[3] = {1e6, 1e6, 1e6 };
  for ( int i = 0 ; i < vertices_xyz.length()/3 ; i++ )
  {
      for ( int j = 0 ; j < 3 ; j++ )
      {
          fmax[j] = __max( fmax[j], vertices_xyz.at(3*i+j) );
          fmin[j] = __min( fmin[j], vertices_xyz.at(3*i+j) );
      }
      qDebug() << " xyz " << vertices_xyz.at(3*i+0) << " " << vertices_xyz.at(3*i+1) << " " << vertices_xyz.at(3*i+2);
//      qDebug() << " max " << fmax[0] << " " << fmax[1] << " " << fmax[2];
//      qDebug() << " min " << fmin[0] << " " << fmin[1] << " " << fmin[2];
  }

    // center
  for ( int i = 0 ; i < vertices_xyz.length()/3 ; i++ )
  {
      for ( int j = 0 ; j < 3 ; j++ )
      {
          vertices_xyz[3*i+j] -= (fmax[j]+fmin[j])/2;
      }
  }

  float fsize = 0;
  for ( int j = 0 ; j < 3 ; j++ )
  if ( ( fmax[j]-fmin[j] ) > fsize )
      fsize = fmax[j]-fmin[j];


  float fscale = 12.0/( fsize );
  for ( int i = 0 ; i < vertices_xyz.length() ; i++ )
      vertices_xyz[i] *= fscale*512;

  QFileInfo fileInfo(file_in.fileName());
  QString strObjectName = fileInfo.baseName().toUpper().replace(" ", "_");
  fileName.append(".h");
  QFile file_out(fileName);

  if (!file_out.open(QIODevice::WriteOnly | QIODevice::Text))
    return;

  qDebug() << "Object name " << strObjectName;
  QString strOutput;

    strOutput.append( "#ifndef __" + strObjectName + "_MODEL__H__\n" );
    strOutput.append( "#define __" + strObjectName + "_MODEL__H__\n\n" );

        // XYZ vertices
    strOutput.append( "#define " + strObjectName + "_VERTEX_COUNT " );
    strOutput.append( QString().sprintf( "%d\n", vertices_xyz.size()/3 ) );
    strOutput.append( "const S3L_Unit " + strObjectName.toLower() + "Vertices["+ strObjectName + "_VERTEX_COUNT * 3] = { \n" );
    for ( int i = 0 ; i < vertices_xyz.size()/3 ; i++ )
    {
        strOutput.append( QString().sprintf( "\t%5d,%5d,%5d,\n",
                          (int)(vertices_xyz[3*i+0]),
                          (int)(vertices_xyz[3*i+1]),
                          (int)(vertices_xyz[3*i+2]) ) );
    }
    strOutput.append( "\n}; // " + strObjectName.toLower() + "Vertices \n\n" );

        // faces vertices index
    strOutput.append( "#define " + strObjectName + "_TRIANGLE_COUNT " );
    strOutput.append( QString().sprintf( "%d\n", faces_vertices_index.size()/3 ) );
    strOutput.append( "const S3L_Index " + strObjectName.toLower() + "TriangleIndices["+ strObjectName + "_TRIANGLE_COUNT * 3] = { \n" );
    for ( int i = 0 ; i < faces_vertices_index.size()/3 ; i++ )
    {
        strOutput.append( QString().sprintf( "\t%5d,%5d,%5d,\n",
                          faces_vertices_index[3*i+0],
                          faces_vertices_index[3*i+1],
                          faces_vertices_index[3*i+2] ) );
    }
    strOutput.append( "\n}; // " + strObjectName.toLower() + "TriangleIndices \n\n" );



        // UV textures coordinates
    strOutput.append( "#define " + strObjectName + "_UV_COUNT " );
    strOutput.append( QString().sprintf( "%d\n", textcord_uv.size()/2 ) );
    strOutput.append( "const S3L_Unit " + strObjectName.toLower() + "UVs["+ strObjectName + "_UV_COUNT * 2] = { \n" );
    for ( int i = 0 ; i < textcord_uv.size()/2 ; i++ )
    {
        strOutput.append( QString().sprintf( "\t%5d,%5d,\n",
                          (int)(textcord_uv[2*i+0]*512),
                          (int)(textcord_uv[2*i+1]*512) ) );
    }
    strOutput.append( "\n}; // " + strObjectName.toLower() + "UVs \n\n" );

        // UV index
    strOutput.append( "#define " + strObjectName + "_UV_INDEX_COUNT " );
    strOutput.append( QString().sprintf( "%d\n", textures_uv_index.size()/3 ) );
    strOutput.append( "const S3L_Index " + strObjectName.toLower() + "UVIndices["+ strObjectName + "_UV_INDEX_COUNT * 3] = { \n" );
    for ( int i = 0 ; i < textures_uv_index.size()/3 ; i++ )
    {
        strOutput.append( QString().sprintf( "\t%5d,%5d,%5d,\n",
                          textures_uv_index[3*i+0],
                          textures_uv_index[3*i+1],
                          textures_uv_index[3*i+2] ) );
    }
    strOutput.append( "\n}; // " + strObjectName.toLower() + "UVIndices \n\n" );

        // object definition

    strOutput.append( "S3L_Model3D " + strObjectName.toLower() + "Model;\n" );
    strOutput.append( "void " + strObjectName.toLower() + "ModelInit()\n{\n" );
    strOutput.append( "  S3L_initModel3D( \n" );
        strOutput.append( "    "+strObjectName.toLower() + "Vertices,\n" );
        strOutput.append( "    "+strObjectName.toUpper() + "_VERTEX_COUNT,\n" );
        strOutput.append( "    "+strObjectName.toLower() + "TriangleIndices,\n" );
        strOutput.append( "    "+strObjectName.toUpper() + "_TRIANGLE_COUNT,\n" );
        strOutput.append( "    &" + strObjectName.toLower() + "Model);\n}\n\n" );

  qDebug() << "Success parsing " << fileName.toStdString().data();

  qDebug() << "static const uint32_t vertexCount {" << vertices_xyz.size()/3 << "};\n";
  qDebug() << "static const uint32_t faceCount {" << faces_vertices_index.size()/3 << "};\n";
    strOutput.append( "#endif // of  __" + strObjectName + "_MODEL__H__\n" );
    file_out.write( strOutput.toUtf8().data() );

    file_out.close();
}


void MainWindow::on_actionConvert_object_triggered()
{
    convert2Obj();
}

