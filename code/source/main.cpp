/*===========================================================================
   Program Entry Point

   Dominik Deak
  ===========================================================================*/

#ifndef ___MAIN_CPP___
#define ___MAIN_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "form_window.h"
#include "main.h"


/*---------------------------------------------------------------------------
   Creates a log file.
  ---------------------------------------------------------------------------*/
void MainCreateLogFile(void)
   {
   QDir Dir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);

   QString DirPath = Dir.absolutePath();

   if (!Dir.exists(DirPath) && !Dir.mkpath(DirPath)) {return;}

   DirPath = Dir.filePath(NAMESPACE_PROJECT::Debug::FileName);
   QByteArray Path = DirPath.toAscii();

   NAMESPACE_PROJECT::Debug::Open(Path.constData());
   }

/*---------------------------------------------------------------------------
   Sets the asset directories for the application.
  ---------------------------------------------------------------------------*/
void MainSetAssetDir(void)
   {
   #if defined (MACOSX)
      QDir Dir = QCoreApplication::applicationDirPath();
   #else
      QDir Dir = QDir::current();
   #endif

   QString AssetPath = Dir.absoluteFilePath(NAMESPACE_PROJECT::File::TextureDir);
   AssetPath = QDir::cleanPath(AssetPath);
   QByteArray Path = AssetPath.toAscii();

   if (!Dir.exists(AssetPath))
      {
      debug("Texture directory does not exist: %s\n", Path.constData());
      }
   else
      {
      debug("Texture path: %s\n", Path.constData());
      NAMESPACE_PROJECT::File::Path::SetTexture(Path.constData());
      }

   AssetPath = Dir.absoluteFilePath(NAMESPACE_PROJECT::File::ShaderDir);
   AssetPath = QDir::cleanPath(AssetPath);
   Path = AssetPath.toAscii();

   if (!Dir.exists(AssetPath))
      {
      debug("Shader directory does not exist: %s\n", Path.constData());
      }
   else
      {
      debug("Shader path: %s\n", Path.constData());
      NAMESPACE_PROJECT::File::Path::SetShader(Path.constData());
      }
   }

/*---------------------------------------------------------------------------
   Program entry point.
  ---------------------------------------------------------------------------*/
int cdeclare main(int argc, char** argv)
   {
   int Error = 0;

   Q_INIT_RESOURCE(kfx_resource);
   
   QApplication Application(argc, argv);

   try {
      Application.setApplicationName(NAMESPACE_PROJECT::AppName);
      Application.setApplicationVersion(NAMESPACE_PROJECT::AppVersion);
      Application.setOrganizationName(NAMESPACE_PROJECT::AppOrg);
      Application.setOrganizationDomain(NAMESPACE_PROJECT::AppAuthorWeb);

      MainCreateLogFile();
      MainSetAssetDir();

      if (!QGLFormat::hasOpenGL())
         {throw dexception("This system does not support OpenGL.");}

      if (!QGLFramebufferObject::hasOpenGLFramebufferObjects())
         {throw dexception("This system does not support OpenGL framebuffer objects.");}

      FormWindow Window;
      Window.show();

      Error = Application.exec();
      }

   catch (std::exception &e)
      {
      QMessageBox::information(nullptr, NAMESPACE_PROJECT::AppName, e.what());
      Error = MAIN_EXIT_ERROR;
      }

   catch (...)
      {
      QMessageBox::information(nullptr, NAMESPACE_PROJECT::AppName, "Trapped an unhandled exception.");
      Error = MAIN_EXIT_ERROR;
      }

   NAMESPACE_PROJECT::Debug::Close();

   return Error;
   }


//==== End of file ===========================================================
#endif
