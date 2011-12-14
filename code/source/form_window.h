/*===========================================================================
   Main Aplication Window Class

   Dominik Deak

   Note: The Meta-Object Compiler must be run on this header to generate 
   the required C++ file to allow interfacing between objects.
  ===========================================================================*/

#ifndef ___FORM_WINDOW_H___
#define ___FORM_WINDOW_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "buffers.h"
#include "thread_capture.h"
#include "common.h"
#include "glwidget.h"
#include "thread_kinect.h"
#include "ui_form_window.h"


/*---------------------------------------------------------------------------
  Classes
  ---------------------------------------------------------------------------*/
class FormWindow : public QMainWindow
   {
   //---- Qt specific ----
   Q_OBJECT

   //---- Constants and definitions ----
   public:

   static const int TimeDetect = 500;              //Timer interval for detecting the device, in ms
   static const int TimeActive = 16;               //Timer interval when the device is active, in ms
   static const int TimeDeviceKill = 3000;         //Wait time for the device thread before terminating, in ms
   static const char* FilePrefixVideo;             //File name prefix for depth buffer streaming
   static const char* FilePrefixDepth;             //File name prefix for video buffer streaming
   static const char* FilePrefixMesh;              //File name prefix for mesh streaming

   //---- Member data ----
   private:

   Ui::WindowMain UI;                              //Form user interface classes
   GLWidget* WidgetVideo;                          //Video OpenGL widget
   GLWidget* WidgetDepth;                          //Depth buffer OpenGL widget
   QLabel* StatusDevice;                           //Status bar for device

   NAMESPACE_PROJECT::Buffers Buffer;              //The actual video and depth frames
   KinectThread* Device;                           //Thread for handling the kinect device

   CaptureThread::CapFormat FileFormat;            //Sream capture file format
   bool FileCompress;                              //Apply data compression on file
   QString Path;                                   //Directory where the frames are streamed

   //---- Methods ----
   public:

   FormWindow(QMainWindow* Parent = nullptr);
   ~FormWindow(void);

   private:

   void CentreWindow(void);

   void closeEvent(QCloseEvent* Event);
   void showEvent(QShowEvent* Event);
   void resizeEvent(QResizeEvent* Event);
   void keyPressEvent(QKeyEvent* Event);

   void EnableWidgets(bool State);

   void EnableStreaming(bool State);
   void EnableFileFormat(bool State);
   void EnableButtonRecord(void);
   void EnableButtonStop(void);
   void EnableButtonRecordStreamOptions(void);
   void EnableFilters(bool State);
   void EnableVideoSource(bool State);
   void EnableColourPicker(bool State);
   void EnableCalibration(bool State);
   void EnableVideoFilterWidgets(void);
   void EnableNearSlider(bool State);
   void EnableFarSlider(bool State);
   void EnableClipSlider(bool State);
   void EnableDepthClippingMode(bool State);
   void EnableDepthPalette(bool State);
   void EnableDepthTransform(bool State);

   void UpdateCalibration(float X, float Y);
   void UpdateSlider(QSlider* Slider, QLabel* Label, const QString &Name, float Value, float Max);

   bool OpenDirectory(void);

   private slots:

   void MenuActionNew(void);
   void MenuActionQuit(void);
   void MenuActionHelp(void);
   void MenuActionAbout(void);

   void ButtonActionRecord(void);
   void ButtonActionStop(void);

   void CheckBoxActionStreamVideo(void);
   void CheckBoxActionStreamDepth(void);
   void CheckBoxActionStreamMesh(void);

   void CheckBoxActionSyncFrames(void);

   void RadioButtonActionTGA(void);
   void RadioButtonActionTGARLE(void);
   void RadioButtonActionPNG(void);

   void RadioButtonActionCaptureRGB(void);
   void RadioButtonActionCaptureBayer(void);
   void RadioButtonActionCaptureIR(void);

   void ButtonActionFilter01(void);
   void ButtonActionFilter02(void);
   void ButtonActionFilter03(void);
   void ButtonActionFilter04(void);
   void ButtonActionFilter05(void);
   void ButtonActionFilter06(void);
   void ButtonActionFilter07(void);
   void ButtonActionFilter08(void);
   void ButtonActionFilter09(void);
   void ButtonActionFilter10(void);

   void ButtonActionColourPicker(void);

   void SliderActionDepthNear(int Value);
   void SliderActionDepthFar(int Value);
   void SliderActionDepthClip(int Value);

   void RadioButtonActionDepthEraseBack(void);
   void RadioButtonActionDepthEraseFront(void);
   void RadioButtonActionDepthClampBack(void);
   void RadioButtonActionDepthClampFront(void);

   void RadioButtonActionDepthPaletteGrey(void);
   void RadioButtonActionDepthPaletteThermal(void);
   void RadioButtonActionDepthPaletteSpectrum(void);
   void RadioButtonActionDepthPaletteSaturate(void);

   void CheckBoxActionDepthTransform(void);
   void SpinBoxActionRoomLength(int Value);

   void DeviceEnableStreams(void);
   void DeviceConnected(bool State);
   void DeviceError(QString Message);
   void RenderError(QString Message);
   void CaptureError(QString Message);
   };


//==== End of file ===========================================================
#endif
