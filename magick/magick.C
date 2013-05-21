
#include <dirent.h>

#define MAGICKCORE_QUANTUM_DEPTH 16
#define MAGICKCORE_HDRI_ENABLE 0
#include <wand/MagickWand.h>

#define WORKER
#include "Greenhouse.h"

struct Listener  :  public Thing
{ public:

  const Str poolname;
  MagickWand *magick_wand;

  Listener (Str pool)  :  Thing (), poolname (pool)
    { ParticipateInPool (poolname);
      INFORM ("Participating in pool '" + poolname + "'");

      ListenForDescrip ("image-request");
      ListenForDescrip ("image-list-request");

      MagickWandGenesis ();
      magick_wand = NewMagickWand ();
    }

  ~Listener ()
    { magick_wand = DestroyMagickWand (magick_wand);
      MagickWandTerminus();
    }

  void Metabolize (const Protein &p)
    { INFORM (p);

      if (HasDescrip (p, "image-request"))
        { INFORM ("image-request protein received, responding...");

          Slaw pathSlaw = p . Ingests() . Find ("path");
          Str path = pathSlaw . ToPrintableString ();

          MagickBooleanType status = MagickReadImage (magick_wand, path);
          if (status == MagickFalse)
            { return; }

          status = MagickScaleImage (magick_wand, 80, 80);
          status = MagickFlipImage (magick_wand);

          unsigned char buf[19200];
          status = MagickExportImagePixels (magick_wand, 0, 0, 80, 80, "RGB", CharPixel, buf);

          ImageClot *image_clot = ImageClot::NewWithContents (80, 80, 3, buf);
          if (image_clot)
            { image_clot->SetSourceName (path);
              Protein response = ProteinWithDescrip ("image-result");
              AppendDescrip (response, path);
              AppendIngest (response, "data", image_clot);
              Deposit (response, poolname);
            }
          else
            WARN ("Couldn't send response because we failed to load an image.");
        }
      else if (HasDescrip (p, "image-list-request"))
        { INFORM ("image-list-request protein received, responding...");

          Slaw pathSlaw = p . Ingests() . Find ("path");
          Str path = pathSlaw . ToPrintableString ();

          Trove <Str> paths;

          const char *exts[] = { "jpg", "png" };
          const int EXTS_COUNT = 2;

          // Get the contents of the specified directory path
          struct dirent *dp;
          DIR *dir = opendir (path . utf8());
          while ((dp = readdir (dir)) != NULL)
            { char *ext = strrchr (dp->d_name, '.');
              if (ext)
                { for (int i = 0; i < EXTS_COUNT; ++i)
                    { if (strcmp (exts[i], ext + 1) == 0)
                        paths . Append (Str (dp -> d_name));
                    }
                }
            }
          closedir (dir);

          Protein response = ProteinWithDescrip ("image-list-result");
          AppendIngest (response, "entries", paths);

          // Make sure the directory path has a slash appended
          if (path[path.Length() - 1] != '/')
            { path.Append('/'); }
          AppendIngest (response, "directory-path", path);

          Deposit (response, poolname);
        }
    }
};


void Setup ()
{ int64 p_pos = args . Find ("-p");
  if (p_pos > -1 && args . Count () > p_pos + 1)
    new Listener (args[p_pos + 1]);
  else
    new Listener ("magick");
}
