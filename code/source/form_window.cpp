/*===========================================================================
   Main Aplication Window Class

   Dominik Deak
  ===========================================================================*/

#ifndef ___FORM_WINDOW_CPP___
#define ___FORM_WINDOW_CPP___

                                                         
/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "filter.h"
#include "filter_fatty.h"
#include "filter_lines.h"
#include "filter_solids.h"
#include "filter_nmap.h"
#include "filter_palette.h"
#include "form_window.h"
#include "thread_capture.h"
#include "thread_kinect.h"


/*---------------------------------------------------------------------------
   Constructor.
  ---------------------------------------------------------------------------*/
const char* FormWindow::FilePrefixVideo = "video";
const char* FormWindow::FilePrefixDepth = "depth";
const char* FormWindow::FilePrefixMesh = "mesh";


/*---------------------------------------------------------------------------
   Constructor.
  ---------------------------------------------------------------------------*/
FormWindow::FormWindow(QMainWindow* Parent) : QMainWindow(Parent)
   {
   //Reset member data
   WidgetVideo = nullptr;
   WidgetDepth = nullptr;
   StatusDevice = nullptr;
   Device = nullptr;
   FileFormat = CaptureThread::FormatTGA;
   FileCompress = false;

   //Setup user interface
   UI.setupUi(this);

   //Move window to centre of desktop
   CentreWindow();

   //Window title
   setWindowTitle(NAMESPACE_PROJECT::AppName);

   //Create OpenGL widgets
   WidgetVideo = new GLWidget(this, UI.FrameVideo, Buffer);
   WidgetDepth = new GLWidget(this, UI.FrameDepth, Buffer);

   //Attach default filters
   WidgetVideo->AttachFilter(new NAMESPACE_PROJECT::Filter());
   WidgetDepth->AttachFilter(new NAMESPACE_PROJECT::FilterPalette(NAMESPACE_PROJECT::FilterPalette::Saturate));

   //Update GL widgets with sync flag
   CheckBoxActionSyncFrames();

   //Create a new thread for the device
   Device = new KinectThread(this, WidgetVideo, WidgetDepth, Buffer);
   Device->start();

   //Update GUI controls related to depth
   Device->SetMax((float)UI.SpinBoxRoomLength->value());
   UpdateSlider(UI.SliderDepthNear, UI.LabelDepthFront, "Front", Device->GetNear(), Device->GetMax());
   UpdateSlider(UI.SliderDepthFar, UI.LabelDepthBack, "Back", Device->GetFar(), Device->GetMax());

   //Deactivate widgets for the moment
   EnableWidgets(false);

   //Hide or disable some widgets not implemented yet
   UI.CheckBoxStreamMesh->setHidden(true);

   //Setup staus bar
   QStatusBar* StatusBar = statusBar();
   if (StatusBar != nullptr) 
      {
      StatusDevice = new QLabel;
      StatusBar->addWidget(StatusDevice);
      StatusDevice->setMargin(2);
      StatusDevice->setFrameShape(QFrame::NoFrame);
      StatusDevice->setText("Device: Not detected");
      }
   }

/*---------------------------------------------------------------------------
   Destructor.
  ---------------------------------------------------------------------------*/
FormWindow::~FormWindow(void)
   {
   if (Device->isRunning())
      {
      Device->stop();

      if (!Device->wait(TimeDeviceKill))
         {
         debug("Kinect device thread is still running, attempting to terminate.\n");
         Device->terminate();
         }
      }
   
   delete Device;
   delete StatusDevice;
   }

/*---------------------------------------------------------------------------
   Moves window to the centre of the desktop.
  ---------------------------------------------------------------------------*/
void FormWindow::CentreWindow(void)
   {
   QDesktopWidget* Desktop = QApplication::desktop();
   if (Desktop == nullptr) {return;}

   int Screen = Desktop->screenNumber(this);
   QRect DesktopRect = Desktop->screenGeometry(Screen);
   QRect WindowRect = geometry();

   int X = (DesktopRect.width() - WindowRect.width()) / 2;
   int Y = (DesktopRect.height() - WindowRect.height()) / 2;

   X = NAMESPACE_PROJECT::Math::Max(X, DesktopRect.x());
   Y = NAMESPACE_PROJECT::Math::Max(Y, DesktopRect.y());

   move(X, Y);
   }

/*---------------------------------------------------------------------------
   Window event handling.
  ---------------------------------------------------------------------------*/
void FormWindow::closeEvent(QCloseEvent* Event)
   {
   if (Device->isRunning()) {Device->stop();}

   Event->accept();
   }

//Show events
void FormWindow::showEvent(QShowEvent* Event)
   {
   if (!isVisible()) {return;}
   WidgetVideo->UpdateGeometry(UI.FrameVideo->rect());
   WidgetDepth->UpdateGeometry(UI.FrameDepth->rect());

   Event->accept();
   }

//Resize events
void FormWindow::resizeEvent(QResizeEvent* Event)
   {
   WidgetVideo->UpdateGeometry(UI.FrameVideo->rect());
   WidgetDepth->UpdateGeometry(UI.FrameDepth->rect());

   Event->accept();
   }

//Keyboard events
void FormWindow::keyPressEvent(QKeyEvent* Event)
   {
   switch (Event->key()) 
      {
      case Qt::Key_S : UpdateCalibration(0.0f, -1.0f); break;
      case Qt::Key_W : UpdateCalibration(0.0f, 1.0f); break;
      case Qt::Key_A : UpdateCalibration(1.0f, 0.0f); break;
      case Qt::Key_D : UpdateCalibration(-1.0f, 0.0f); break;
      case Qt::Key_1 : ButtonActionFilter01(); break;
      case Qt::Key_2 : ButtonActionFilter02(); break;
      case Qt::Key_3 : ButtonActionFilter03(); break;
      case Qt::Key_4 : ButtonActionFilter04(); break;
      case Qt::Key_5 : ButtonActionFilter05(); break;
      case Qt::Key_6 : ButtonActionFilter06(); break;
      case Qt::Key_7 : ButtonActionFilter07(); break;
      case Qt::Key_8 : ButtonActionFilter08(); break;
      case Qt::Key_Escape : close(); break;
      default : break;
      }
   }

/*---------------------------------------------------------------------------
   Enables or disables device streams, depending on what the installed
   filters require.
  ---------------------------------------------------------------------------*/
void FormWindow::DeviceEnableStreams(void)
   {
   if (!Device->Connected()) {return;}

   bool EnableVideo = false;
   bool EnableDepth = false;

   const NAMESPACE_PROJECT::Filter* Filter = WidgetVideo->GetFilter();
   
   if (Filter != nullptr)
      {
      EnableVideo |= Filter->UsesVideo();
      EnableDepth |= Filter->UsesDepth();
      }

   Filter = WidgetDepth->GetFilter();

   if (Filter != nullptr)
      {
      EnableVideo |= Filter->UsesVideo();
      EnableDepth |= Filter->UsesDepth();
      }
   
   if (EnableVideo) {Device->StartVideo();} else {Device->StopVideo();}
   if (EnableDepth) {Device->StartDepth();} else {Device->StopDepth();}
   
   EnableVideoSource(EnableVideo);
   }

/*---------------------------------------------------------------------------
   Receives connection status signals from the kinect device.
  ---------------------------------------------------------------------------*/
void FormWindow::DeviceConnected(bool State)
   {
   if (StatusDevice != nullptr)
      {
      if (State) {StatusDevice->setText("Device: Connected");}
      else {StatusDevice->setText("Device: Not detected");}
      }

   EnableWidgets(State);
   
   if (State) 
      {
      DeviceEnableStreams();
      EnableVideoFilterWidgets();
      EnableButtonRecord();
      }
   }

/*---------------------------------------------------------------------------
   Traps a device error signal.
  ---------------------------------------------------------------------------*/
void FormWindow::DeviceError(QString Message)
   {
   if (Device->isRunning()) {Device->stop();}

   close();

   QMessageBox::information(nullptr, NAMESPACE_PROJECT::AppName, "An error has occured in the Kinect device thread:\n\n" + Message);

   QByteArray String = Message.toAscii();
   debug("%s\n", String.constData()); 
   }

/*---------------------------------------------------------------------------
   Traps a render error signal.
  ---------------------------------------------------------------------------*/
void FormWindow::RenderError(QString Message)
   {
   close();

   QMessageBox::information(nullptr, NAMESPACE_PROJECT::AppName, "An error has occured in the rendering function:\n\n" + Message);

   QByteArray String = Message.toAscii();
   debug("%s\n", String.constData()); 
   }

/*---------------------------------------------------------------------------
   Traps a capture error signal.
  ---------------------------------------------------------------------------*/
void FormWindow::CaptureError(QString Message)
   {
   QMessageBox::information(nullptr, NAMESPACE_PROJECT::AppName, "An error has occured in the capture thread:\n\n" + Message);

   try {
      WidgetVideo->CaptureClose();
      WidgetDepth->CaptureClose();

      EnableButtonRecord();
      }

   catch (std::exception &e) {debug("%s\n", e.what());}
   catch (...) {debug("Trapped an unhandled exception.\n");}
   }

/*---------------------------------------------------------------------------
   Enables or disables window widgets, depending on State. 
  ---------------------------------------------------------------------------*/
void FormWindow::EnableWidgets(bool State)
   {
   //Menu items
   UI.ActionNew->setEnabled(State);
   
   EnableStreaming(State);
   EnableFileFormat(State);
   EnableFilters(State);
   EnableVideoSource(State);
   EnableColourPicker(State);
   EnableCalibration(State);
   EnableNearSlider(State);
   EnableFarSlider(State);
   EnableClipSlider(State);
   EnableDepthClippingMode(State);
   EnableDepthPalette(State);
   EnableDepthTransform(State);
   }

/*---------------------------------------------------------------------------
   Enable/disable streaming related widgets.
  ---------------------------------------------------------------------------*/
void FormWindow::EnableStreaming(bool State)
   {
   UI.ButtonRecord->setEnabled(State);
   UI.ButtonStop->setEnabled(State);

   UI.CheckBoxStreamVideo->setEnabled(State);
   UI.CheckBoxStreamDepth->setEnabled(State);
   UI.CheckBoxStreamMesh->setEnabled(State);

   UI.CheckBoxSyncFrames->setEnabled(State);
   }

/*---------------------------------------------------------------------------
   Enable/disable fileformat related widgets.
  ---------------------------------------------------------------------------*/
void FormWindow::EnableFileFormat(bool State)
   {
   UI.RadioButtonTGA->setEnabled(State);
   UI.RadioButtonTGARLE->setEnabled(State);
   UI.RadioButtonPNG->setEnabled(State);
   }

/*---------------------------------------------------------------------------
   Enables the record button and associated stuff, disables stop button.
  ---------------------------------------------------------------------------*/
void FormWindow::EnableButtonRecord(void)
   {
   UI.ButtonRecord->setEnabled(true);
   UI.ButtonStop->setEnabled(false);

   UI.ActionNew->setEnabled(true);

   UI.CheckBoxStreamVideo->setEnabled(true);
   UI.CheckBoxStreamDepth->setEnabled(true);
   UI.CheckBoxStreamMesh->setEnabled(true);

   EnableFileFormat(true);

   Device->SetLED(NAMESPACE_PROJECT::Kinect::LedGreen);
   }

/*---------------------------------------------------------------------------
   Enables the stop button, disables record button and associated stuff.
  ---------------------------------------------------------------------------*/
void FormWindow::EnableButtonStop(void)
   {
   UI.ButtonRecord->setEnabled(false);
   UI.ButtonStop->setEnabled(true);

   UI.ActionNew->setEnabled(false);

   UI.CheckBoxStreamVideo->setEnabled(false);
   UI.CheckBoxStreamDepth->setEnabled(false);
   UI.CheckBoxStreamMesh->setEnabled(false);

   EnableFileFormat(false);

   Device->SetLED(NAMESPACE_PROJECT::Kinect::LedRed);
   }

/*---------------------------------------------------------------------------
   Helper function that updates the record button based on the check box 
   state of streaming options.
  ---------------------------------------------------------------------------*/
void FormWindow::EnableButtonRecordStreamOptions(void)
   {
   bool State;
   
   State  = UI.CheckBoxStreamVideo->checkState() == Qt::Checked;
   State |= UI.CheckBoxStreamDepth->checkState() == Qt::Checked;
   State |= UI.CheckBoxStreamMesh->checkState() == Qt::Checked;

   if (State ^ UI.ButtonRecord->isEnabled())
      {
      UI.ButtonRecord->setEnabled(State);
      }
   }

/*---------------------------------------------------------------------------
   Enable/disable filter buttons.
  ---------------------------------------------------------------------------*/
void FormWindow::EnableFilters(bool State)
   {
   UI.ButtonFilter01->setEnabled(State);
   UI.ButtonFilter02->setEnabled(State);
   UI.ButtonFilter03->setEnabled(State);
   UI.ButtonFilter04->setEnabled(State);
   UI.ButtonFilter05->setEnabled(State);
   UI.ButtonFilter06->setEnabled(State);
   UI.ButtonFilter07->setEnabled(State);
   UI.ButtonFilter08->setEnabled(State);
   UI.ButtonFilter09->setEnabled(State);
   UI.ButtonFilter10->setEnabled(State);
   }

/*---------------------------------------------------------------------------
   Enable/disable video source group.
  ---------------------------------------------------------------------------*/
void FormWindow::EnableVideoSource(bool State)
   {
   UI.RadioButtonCaptureRGB->setEnabled(State);
   UI.RadioButtonCaptureBayer->setEnabled(State);
   UI.RadioButtonCaptureIR->setEnabled(State);
   }

/*---------------------------------------------------------------------------
   Enable/disable colour picker group.
  ---------------------------------------------------------------------------*/
void FormWindow::EnableColourPicker(bool State)
   {
   UI.FrameColourBox->setEnabled(State);
   UI.ButtonColourPicker->setEnabled(State);
   }

/*---------------------------------------------------------------------------
   Enable/disable calibration group.
  ---------------------------------------------------------------------------*/
void FormWindow::EnableCalibration(bool State)
   {
   UI.RadioButtonCalibrationTrans->setEnabled(State);
   UI.RadioButtonCalibrationScale->setEnabled(State);
   }

/*---------------------------------------------------------------------------
   Enables or disables the video filter related group boxex, depending on the 
   current capability of the video filter.
  ---------------------------------------------------------------------------*/
void FormWindow::EnableVideoFilterWidgets(void)
   {
   NAMESPACE_PROJECT::Filter* Filter = WidgetVideo->GetFilter();
   
   //Colour picker
   EnableColourPicker(Filter != nullptr && Filter->UsesColour());

   if (Filter != nullptr)
      {
      QColor WidgetColour;
      NAMESPACE_PROJECT::vector4f FilterColour = Filter->GetColour();
      WidgetColour.setRgbF((qreal)FilterColour.R, (qreal)FilterColour.G, (qreal)FilterColour.B, (qreal)FilterColour.A);

      QPalette Palette = UI.FrameColourBox->palette();
      Palette.setColor(backgroundRole(), WidgetColour);
      UI.FrameColourBox->setPalette(Palette);
      }

   //Calibration
   EnableCalibration(Filter != nullptr && Filter->UsesCal());
   }

/*---------------------------------------------------------------------------
   Enable/disable depth sliders.
  ---------------------------------------------------------------------------*/
//Front plane slider
void FormWindow::EnableNearSlider(bool State)
   {
   UI.LabelDepthFront->setEnabled(State);
   UI.LabelDepthNear1->setEnabled(State);
   UI.SliderDepthNear->setEnabled(State);
   UI.LabelDepthFar1->setEnabled(State);
   }

//Back plane slider
void FormWindow::EnableFarSlider(bool State)
   {
   UI.LabelDepthBack->setEnabled(State);
   UI.LabelDepthNear2->setEnabled(State);
   UI.SliderDepthFar->setEnabled(State);
   UI.LabelDepthFar2->setEnabled(State);
   }

   //Clipper slider
void FormWindow::EnableClipSlider(bool State)
   {
   UI.LabelDepthClip->setEnabled(State);
   UI.LabelDepthNear3->setEnabled(State);
   UI.SliderDepthClip->setEnabled(State);
   UI.LabelDepthFar3->setEnabled(State);
   }

/*---------------------------------------------------------------------------
   Enable/disable depth clipping mode.
  ---------------------------------------------------------------------------*/
void FormWindow::EnableDepthClippingMode(bool State)
   {
   UI.RadioButtonDepthClipEraseBack->setEnabled(State);
   UI.RadioButtonDepthClipEraseFront->setEnabled(State);
   UI.RadioButtonDepthClipClampBack->setEnabled(State);
   UI.RadioButtonDepthClipClampFront->setEnabled(State);
   }

/*---------------------------------------------------------------------------
   Enable/disable depth clipping mode.
  ---------------------------------------------------------------------------*/
void FormWindow::EnableDepthPalette(bool State)
   {
   UI.RadioButtonDepthGrey->setEnabled(State);
   UI.RadioButtonDepthThermal->setEnabled(State);
   UI.RadioButtonDepthSpectrum->setEnabled(State);
   UI.RadioButtonDepthSaturate->setEnabled(State);
   }

/*---------------------------------------------------------------------------
   Enable/disable depth clipping mode.
  ---------------------------------------------------------------------------*/
void FormWindow::EnableDepthTransform(bool State)
   {
   UI.CheckBoxDepthTransform->setEnabled(State);
   UI.LabelRoomLength->setEnabled(State);
   UI.SpinBoxRoomLength->setEnabled(State);
   }

/*---------------------------------------------------------------------------
   Updates the scaling component of the texture calbiration.
  ---------------------------------------------------------------------------*/
void FormWindow::UpdateCalibration(float X, float Y)
   {
   NAMESPACE_PROJECT::Filter* Filter = WidgetVideo->GetFilter();
   
   if (Filter == nullptr || !Filter->UsesCal()) {return;}
   
   if (!UI.RadioButtonCalibrationScale->isEnabled()) {return;}
   
   if (UI.RadioButtonCalibrationScale->isChecked())
      {
      Filter->AddScale(X, Y);
      }
   else if (UI.RadioButtonCalibrationTrans->isChecked())
      {
      Filter->AddTrans(X, Y);
      }
   }

/*---------------------------------------------------------------------------
   Update sliders for controlling the near and far clipping plane. This 
   function will clamp slider values and change their labels.

   Slider : Slider object to change.
   Label  : Label object associated with slider.
   Name   : Base name of the label.
   Value  : Slider value to set.
   Max    : The maximum range for Value.
  ---------------------------------------------------------------------------*/
void FormWindow::UpdateSlider(QSlider* Slider, QLabel* Label, const QString &Name, float Value, float Max)
   {
   if (Slider == nullptr || Label == nullptr) {return;}
   
   int Range = Slider->maximum() - Slider->minimum();
   
   Slider->setValue((int)((float)Range * (Value / Max)) + Slider->minimum());

   if (UI.CheckBoxDepthTransform->isChecked())
      {
      Label->setText(Name + QString(" (%1 m)").arg(Value, 0, 'f', 2));
      }
   else
      {
      Label->setText(Name);
      }
   }

/*---------------------------------------------------------------------------
   Displays the open directory dialogue. Returns false if the action was
   cancelled.
  ---------------------------------------------------------------------------*/
bool FormWindow::OpenDirectory(void)
   {
   QString Caption = "Select the output directory where you want to save the video streams.";
   QString Dir = Path.size() < 1 ? QDir::homePath() : Path;
   QString NewPath = QFileDialog::getExistingDirectory(this, Caption, Dir);

   if (NewPath.size() > 0)
      {
      Path = NewPath;
      return true;
      }

   return false;
   }

/*---------------------------------------------------------------------------
   Menu actions.
  ---------------------------------------------------------------------------*/
//Create new project
void FormWindow::MenuActionNew(void)
   {
   OpenDirectory();
   }

//Exit application
void FormWindow::MenuActionQuit(void)
   {
   close();
   }

//Help menu
void FormWindow::MenuActionHelp(void)
   {
   QMessageBox::information(nullptr, NAMESPACE_PROJECT::AppName, "This feature is not available at the moment.");
   }

//Display about box
void FormWindow::MenuActionAbout(void)
   {
   QString Title;
   Title += "About ";
   Title += NAMESPACE_PROJECT::AppName;

   QString Message;
   Message += "<b>";
   Message += NAMESPACE_PROJECT::AppName;
   Message += "</b><br>";

   Message += "Version ";
   Message += NAMESPACE_PROJECT::AppVersion;
   Message += "<br><br>";

   Message += NAMESPACE_PROJECT::AppAuthor;
   Message += " <a href=\"http://";
   Message += NAMESPACE_PROJECT::AppAuthorWeb;
   Message += "\">";
   Message += NAMESPACE_PROJECT::AppAuthorWeb;
   Message += "</a><br>";

   Message += NAMESPACE_PROJECT::AppOrg;
   Message += " <a href=\"http://";
   Message += NAMESPACE_PROJECT::AppOrgWeb;
   Message += "\">";
   Message += NAMESPACE_PROJECT::AppOrgWeb;
   Message += "</a>";

   QMessageBox::about(this, Title, Message);
   }

/*---------------------------------------------------------------------------
   Start recording button action.
  ---------------------------------------------------------------------------*/
void FormWindow::ButtonActionRecord(void)
   {
   try {
      /*
      #if defined(DEBUG)
         if (Path.size() < 1) {Path = ".";}
      #else
      */
         if (Path.size() < 1 && !OpenDirectory())
            {
            if (UI.ButtonRecord->isDown()) {UI.ButtonRecord->setDown(false);}
            return;
            }
      /*
      #endif
      */

      bool StartVideo = UI.CheckBoxStreamVideo->checkState() == Qt::Checked;
      bool StartDepth = UI.CheckBoxStreamDepth->checkState() == Qt::Checked;

      if (StartVideo) {WidgetVideo->CaptureOpen(Path, FormWindow::FilePrefixVideo, FileFormat, FileCompress);}
      if (StartDepth) {WidgetDepth->CaptureOpen(Path, FormWindow::FilePrefixDepth, FileFormat, FileCompress);}

      EnableButtonStop();
      }

   catch (std::exception &e) 
      {QMessageBox::information(nullptr, NAMESPACE_PROJECT::AppName, e.what());}

   catch (...) 
      {QMessageBox::information(nullptr, NAMESPACE_PROJECT::AppName, "Trapped an unhandled exception.");}
   }

/*---------------------------------------------------------------------------
   Stop recording button action.
  ---------------------------------------------------------------------------*/
void FormWindow::ButtonActionStop(void)
   {
   try {
      WidgetVideo->CaptureClose();
      WidgetDepth->CaptureClose();

      EnableButtonRecord();
      }

   catch (std::exception &e) 
      {QMessageBox::information(nullptr, NAMESPACE_PROJECT::AppName, e.what());}

   catch (...) 
      {QMessageBox::information(nullptr, NAMESPACE_PROJECT::AppName, "Trapped an unhandled exception.");}
   }

/*---------------------------------------------------------------------------
   Streaming check box actions.
  ---------------------------------------------------------------------------*/
//Stream RGB buffer
void FormWindow::CheckBoxActionStreamVideo(void)
   {
   EnableButtonRecordStreamOptions();
   }

//Stream depth buffer
void FormWindow::CheckBoxActionStreamDepth(void)
   {
   EnableButtonRecordStreamOptions();
   }

//Stream mesh data
void FormWindow::CheckBoxActionStreamMesh(void)
   {
   EnableButtonRecordStreamOptions();
   }

/*---------------------------------------------------------------------------
   Frame sync checkbox.
  ---------------------------------------------------------------------------*/
void FormWindow::CheckBoxActionSyncFrames(void)
   {
   bool Sync = UI.CheckBoxSyncFrames->checkState() == Qt::Checked;
   WidgetVideo->CaptureSync(Sync);
   WidgetDepth->CaptureSync(Sync);
   }

/*---------------------------------------------------------------------------
   Radio buttons for changing the file format.
  ---------------------------------------------------------------------------*/
//Stream as uncompressed TGA
void FormWindow::RadioButtonActionTGA(void)
   {
   FileFormat = CaptureThread::FormatTGA;
   FileCompress = false;
   }

//Stream as compressed TGA
void FormWindow::RadioButtonActionTGARLE(void)
   {
   FileFormat = CaptureThread::FormatTGA;
   FileCompress = true;
   }

//Stream as PNG
void FormWindow::RadioButtonActionPNG(void)
   {
   FileFormat = CaptureThread::FormatPNG;
   FileCompress = true;
   }

/*---------------------------------------------------------------------------
   Radio buttons for changing the device video capture mode.
  ---------------------------------------------------------------------------*/
//Capture as RGB
void FormWindow::RadioButtonActionCaptureRGB(void)
   {
   if (!Device->Connected()) {return;}
   Device->ChangeVideo(NAMESPACE_PROJECT::Kinect::VideoRGB);
   debug("Video source was set to RGB.\n");
   }

//Capture as raw Bayer encoding
void FormWindow::RadioButtonActionCaptureBayer(void)
   {
   if (!Device->Connected()) {return;}
   Device->ChangeVideo(NAMESPACE_PROJECT::Kinect::VideoBayer);
   debug("Video source was set to Bayer.\n");
   }

//Capture infrared
void FormWindow::RadioButtonActionCaptureIR(void)
   {
   if (!Device->Connected()) {return;}
   Device->ChangeVideo(NAMESPACE_PROJECT::Kinect::VideoIR);
   debug("Video source was set to infrared.\n");
   }

/*---------------------------------------------------------------------------
   Filter effects button actions.
  ---------------------------------------------------------------------------*/
void FormWindow::ButtonActionFilter01(void) 
   {
   if (!Device->Connected()) {return;}
   WidgetVideo->AttachFilter(new NAMESPACE_PROJECT::Filter());
   DeviceEnableStreams();
   EnableVideoFilterWidgets();
   }

void FormWindow::ButtonActionFilter02(void) 
   {
   if (!Device->Connected()) {return;}
   WidgetVideo->AttachFilter(new NAMESPACE_PROJECT::FilterLines());
   DeviceEnableStreams();
   EnableVideoFilterWidgets();
   }

void FormWindow::ButtonActionFilter03(void) 
   {
   if (!Device->Connected()) {return;}
   WidgetVideo->AttachFilter(new NAMESPACE_PROJECT::FilterFatty());
   DeviceEnableStreams();
   EnableVideoFilterWidgets();
   }

void FormWindow::ButtonActionFilter04(void) 
   {
   if (!Device->Connected()) {return;}
   WidgetVideo->AttachFilter(new NAMESPACE_PROJECT::FilterSolids(NAMESPACE_PROJECT::FilterSolids::Cubes));
   DeviceEnableStreams();
   EnableVideoFilterWidgets();
   }

void FormWindow::ButtonActionFilter05(void) 
   {
   if (!Device->Connected()) {return;}
   WidgetVideo->AttachFilter(new NAMESPACE_PROJECT::FilterSolids(NAMESPACE_PROJECT::FilterSolids::Spheres));
   DeviceEnableStreams();
   EnableVideoFilterWidgets();
   }

void FormWindow::ButtonActionFilter06(void) 
   {
   if (!Device->Connected()) {return;}
   WidgetVideo->AttachFilter(new NAMESPACE_PROJECT::FilterSolids(NAMESPACE_PROJECT::FilterSolids::CubesTinted));
   DeviceEnableStreams();
   EnableVideoFilterWidgets();
   }

void FormWindow::ButtonActionFilter07(void) 
   {
   if (!Device->Connected()) {return;}
   WidgetVideo->AttachFilter(new NAMESPACE_PROJECT::FilterSolids(NAMESPACE_PROJECT::FilterSolids::SpheresTinted));
   DeviceEnableStreams();
   EnableVideoFilterWidgets();
   }

void FormWindow::ButtonActionFilter08(void) 
   {
   if (!Device->Connected()) {return;}
   WidgetVideo->AttachFilter(new NAMESPACE_PROJECT::FilterSolids(NAMESPACE_PROJECT::FilterSolids::CubesFar));
   DeviceEnableStreams();
   EnableVideoFilterWidgets();
   }

void FormWindow::ButtonActionFilter09(void) 
   {
   if (!Device->Connected()) {return;}
   WidgetVideo->AttachFilter(new NAMESPACE_PROJECT::FilterSolids(NAMESPACE_PROJECT::FilterSolids::SpheresFar));
   DeviceEnableStreams();
   EnableVideoFilterWidgets();
   }

void FormWindow::ButtonActionFilter10(void) 
   {
   if (!Device->Connected()) {return;}
   WidgetVideo->AttachFilter(new NAMESPACE_PROJECT::FilterNMap());
   DeviceEnableStreams();
   EnableVideoFilterWidgets();
   }

/*---------------------------------------------------------------------------
   Opens the colour picker box.
  ---------------------------------------------------------------------------*/
void FormWindow::ButtonActionColourPicker(void)
   {
   NAMESPACE_PROJECT::Filter* Filter = WidgetVideo->GetFilter();
   bool State = Filter != nullptr && Filter->UsesColour();
   if (!State) {debug("The current video filter does not use colours."); return;}

   QColor WidgetColour;
   NAMESPACE_PROJECT::vector4f FilterColour = Filter->GetColour();
   WidgetColour.setRgbF((qreal)FilterColour.R, (qreal)FilterColour.G, (qreal)FilterColour.B, (qreal)FilterColour.A);

   WidgetColour = QColorDialog::getColor(WidgetColour, this);

   FilterColour.Set4((float)WidgetColour.redF(), (float)WidgetColour.greenF(), (float)WidgetColour.blueF(), (float)WidgetColour.alphaF());
   Filter->SetColour(FilterColour);

   QPalette Palette = UI.FrameColourBox->palette();
   Palette.setColor(backgroundRole(), WidgetColour);
   UI.FrameColourBox->setPalette(Palette);
   }

/*---------------------------------------------------------------------------
   Depth buffer sliders.
  ---------------------------------------------------------------------------*/
void FormWindow::SliderActionDepthNear(int Value)
   {
   int Range = UI.SliderDepthNear->maximum() - UI.SliderDepthNear->minimum();
   if (Range < 1) {return;}
   Device->SetNear((float)Value / (float)Range);
   UpdateSlider(UI.SliderDepthNear, UI.LabelDepthFront, "Front", Device->GetNear(), Device->GetMax());
   }

void FormWindow::SliderActionDepthFar(int Value)
   {
   int Range = UI.SliderDepthFar->maximum() - UI.SliderDepthFar->minimum();
   if (Range < 1) {return;}
   Device->SetFar((float)Value / (float)Range);
   UpdateSlider(UI.SliderDepthFar, UI.LabelDepthBack, "Back", Device->GetFar(), Device->GetMax());
   }

void FormWindow::SliderActionDepthClip(int Value)
   {
   int Range = UI.SliderDepthClip->maximum() - UI.SliderDepthClip->minimum();
   if (Range < 1) {return;}
   Device->SetClip((float)Value / (float)Range);
   }

/*---------------------------------------------------------------------------
   Depth clipping mode radio buttons.
  ---------------------------------------------------------------------------*/
void FormWindow::RadioButtonActionDepthEraseBack(void) 
   {
   Device->SetClipMethod(NAMESPACE_PROJECT::Kinect::EraseBack);
   }

void FormWindow::RadioButtonActionDepthEraseFront(void) 
   {
   Device->SetClipMethod(NAMESPACE_PROJECT::Kinect::EraseFront);
   }

void FormWindow::RadioButtonActionDepthClampBack(void) 
   {
   Device->SetClipMethod(NAMESPACE_PROJECT::Kinect::ClampBack);
   }

void FormWindow::RadioButtonActionDepthClampFront(void) 
   {
   Device->SetClipMethod(NAMESPACE_PROJECT::Kinect::ClampFront);
   }

/*---------------------------------------------------------------------------
   Depth palette radio buttons.
  ---------------------------------------------------------------------------*/
void FormWindow::RadioButtonActionDepthPaletteGrey(void)
   {
   if (!Device->Connected()) {return;}
   WidgetDepth->AttachFilter(new NAMESPACE_PROJECT::FilterPalette(NAMESPACE_PROJECT::FilterPalette::Grey));
   DeviceEnableStreams();
   }

void FormWindow::RadioButtonActionDepthPaletteThermal(void)
   {
   if (!Device->Connected()) {return;}
   WidgetDepth->AttachFilter(new NAMESPACE_PROJECT::FilterPalette(NAMESPACE_PROJECT::FilterPalette::Thermal));
   DeviceEnableStreams();
   }

void FormWindow::RadioButtonActionDepthPaletteSpectrum(void)
   {
   if (!Device->Connected()) {return;}
   WidgetDepth->AttachFilter(new NAMESPACE_PROJECT::FilterPalette(NAMESPACE_PROJECT::FilterPalette::Spectrum));
   DeviceEnableStreams();
   }

void FormWindow::RadioButtonActionDepthPaletteSaturate(void)
   {
   if (!Device->Connected()) {return;}
   WidgetDepth->AttachFilter(new NAMESPACE_PROJECT::FilterPalette(NAMESPACE_PROJECT::FilterPalette::Saturate));
   DeviceEnableStreams();
   }

/*---------------------------------------------------------------------------
   Toggle linear transform.
  ---------------------------------------------------------------------------*/
void FormWindow::CheckBoxActionDepthTransform(void)
   {
   if (!Device->Connected()) {return;}
   bool State = UI.CheckBoxDepthTransform->isChecked();
   Device->SetLinear(State);
   UI.SpinBoxRoomLength->setEnabled(State);
   UpdateSlider(UI.SliderDepthNear, UI.LabelDepthFront, "Front", Device->GetNear(), Device->GetMax());
   UpdateSlider(UI.SliderDepthFar, UI.LabelDepthBack, "Back", Device->GetFar(), Device->GetMax());
   }

/*---------------------------------------------------------------------------
   Updates room size.
  ---------------------------------------------------------------------------*/
void FormWindow::SpinBoxActionRoomLength(int Value)
   {
   if (!Device->Connected()) {return;}
   Device->SetMax((float)Value);
   UpdateSlider(UI.SliderDepthNear, UI.LabelDepthFront, "Front", Device->GetNear(), Device->GetMax());
   UpdateSlider(UI.SliderDepthFar, UI.LabelDepthBack, "Back", Device->GetFar(), Device->GetMax());
   }


//==== End of file ===========================================================
#endif
