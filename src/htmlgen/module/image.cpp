#include "image.h"

#include <htmlgen/encode.h>
#include <util/wikidbinterface.h>

const int thumbnail_size = wgThumbLimits[wgDefaultUserOptions.value("thumbsize").toInt()];

const QStringList file_lookup =
{
  "Image",
  "File",
  "Media",
};

CImageModule::CImageModule(const tag_t& tag)
  : filename(encoder(tag.tag_name)),
    link("File:" + filename),
    scale(1.0),
    thumbnail(false),
    width(0),
    height(0),
    align("right"),
    frame(false),
    frameless(false)
{
  Q_ASSERT(file_lookup.contains(tag.tag_namespace));

  QueryThread db;
  vector_t<QVariant> dimensions;
  vector_t<vector_t<QVariant>> results = db.fastQuery(query("media info"), filename);
  if(!results.isEmpty())
    dimensions = results.front();

  exists = !dimensions.isEmpty();

  for(auto pos = tag.attributes.begin(); pos != tag.attributes.end(); ++pos)
  {
    if(!pos->key().isEmpty())
    {
      switch(pos->key())
      {
        case "alt"_hash:
          alt = pos->value();
          break;
        case "link"_hash:
          link = pos->value();
          break;
        case "upright"_hash:
          scale = pos->value().toFloat();
          break;

        default:
          qDebug() << "key:"  << pos->key();
          qDebug() << "value:" << pos->value();
          Q_ASSERT(false);
      }
    }
    else
    {
      switch(pos->value())
      {
        case "left"_hash:
        case "right"_hash:
        case "center"_hash:
          align = pos->value();
          break;
        case "thumb"_hash:
        case "thumbnail"_hash:
          thumbnail = true;
          break;
        case "frame"_hash:
          frame = true;
          break;
        case "frameless"_hash:
          frameless = true;
          break;
        case "none"_hash:
          linebreak = "true";
          break;
        case "baseline"_hash:
          baseline = "false";
          break;
        default:
          try
          {
            int vals[2] = {0};

            if(!pos->value().endsWith("px"))
              throw(pos->value());

            auto str_pos = pos->value().begin();
            auto str_end = pos->value().end();
            --str_end;
            --str_end;

            for(; str_pos != str_end && QChar::isDigit(*str_pos); ++str_pos)
            {
              vals[0] *= 10;
              vals[0] += QChar::digitValue(*str_pos);
            }

            if(str_pos != str_end && *str_pos == 'x')
            {
              for(++str_pos; str_pos != str_end && QChar::isDigit(*str_pos); ++str_pos)
              {
                vals[1] *= 10;
                vals[1] += QChar::digitValue(*str_pos);
              }
            }

            if(str_pos == str_end)
            {
              width  = vals[0];
              height = vals[1];
            }
            else
              throw(pos->value());
          }
          catch(HUString str)
          {
            caption = str;
          }
          break;
      }
    }
  }

  if(exists)
  {
    switch(HUString(dimensions[0]))
    {
      case "DRAWING"_hash:
      case "BITMAP"_hash:
      {{
        register int w = dimensions[1].toInt();
        register int h = dimensions[2].toInt();

        if(!width && !height) // IF width/height are not set OR
        {
          if(thumbnail || frame || frameless)
          {
            width = thumbnail_size * scale;
            height = width * h / w;
          }
          else
          {
            width = w;
            height = h;
          }
        }
        else if(!width)
          width = height * w / h;
        else if(!height)
          height = width * h / w;
      }}
        break;

      default:
        qDebug() << HUString(dimensions[0]);
        Q_ASSERT(false);
    }
  }
#ifdef ENABLE_BUGGY_MODE
  // is this intentional behavior?
  else
  {
    width = thumbnail_size;
  }
#endif
}

void CImageModule::render(CElement& element)
{
  const bool framed = thumbnail || frame;

  if(framed)
  {
    element.becomeChild("div", { "class", "thumb t" + align });
    element.becomeChild("div", { {"class", "thumbinner"}, {"style", "width:"+HUString(width+2)+"px"} });
  }
  else
    element.becomeChild("div", { "class", align });


  if(exists)
  {
// link and image
    element.becomeChild("a", { {"href", "/" + link}, {"class", "image"} });
    if(!framed) // if _only_ frameless
      element.setAttribute("title", caption);
    element.becomeChild("img", { {"src", image_lookup(filename)}, {"width", width}, {"height", height}, {"class", "hoverZoomLink"} });
    if(!alt.isEmpty())
      element.setAttribute("alt", alt);
    if(framed)
      element.setAttribute("class", "thumbImage hoverZoomLink");
    element.becomeParent();
    element.becomeParent();
  }
  else
  {
// link and text
    element.becomeChild("a", { {"href", "/" + link}, {"class", "new"}, {"title", link} });
      element.appendText(link);
    element.becomeParent();
  }

// caption and zoom
  if(thumbnail)
    element.becomeChild("div", {"class", "thumbcaption"});

  if(exists && framed)
  {
    element.becomeChild("div", {"class", "magnify"});
      element.becomeChild("a", { {"href", filename}, {"class", "internal"}, {"title", "Enlarge"} });
        element.appendElement("img", { {"src", "/images/magnify-clip.png"}, {"width", "15"}, {"height", "11"}, {"alt", ""} });
      element.becomeParent();
    element.becomeParent();
  }

  if(thumbnail)
  {
    element.appendText(caption);
    element.becomeParent();
  }

  if(framed)
    element.becomeParent();
  element.becomeParent();
}
