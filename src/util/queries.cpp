#include"queries.h"

#include <soloutils>

const char* query(const char* name)
{
  switch(hash_string(name))
  {
    case "check page exists in namespace"_hash:
      return "SELECT"
             " p.page_title AS 'Page Title',"
             " p.page_namespace AS 'Namespace'"
             " FROM page p"
             " WHERE p.page_namespace = ?"
             " AND p.page_title = ?"
             " LIMIT 1";

    case "check page exists"_hash:
      return "SELECT"
             " p.page_title AS 'Page Title',"
             " p.page_namespace AS 'Namespace'"
             " FROM page p"
             " WHERE p.page_title = ?"
             " LIMIT 1";

    case "latest page"_hash:
      return "SELECT"
             " p.page_title AS 'Page Title',"
             " p.page_namespace AS 'Namespace',"
             " t.old_text AS 'Text'"
             " FROM page p"
             " INNER JOIN revision r"
             " ON p.page_latest = r.rev_id"
             " INNER JOIN text t"
             " ON r.rev_text_id = t.old_id"
             " WHERE p.page_title = ?"
             " LIMIT 1";

    case "latest page in namespace"_hash:
      return "SELECT"
             " p.page_title AS 'Page Title',"
             " p.page_namespace AS 'Namespace',"
             " t.old_text AS 'Text'"
             " FROM page p"
             " INNER JOIN revision r"
             " ON p.page_latest = r.rev_id"
             " INNER JOIN text t"
             " ON r.rev_text_id = t.old_id"
             " WHERE p.page_namespace = ?"
             " AND p.page_title = ?"
             " LIMIT 1";

    case "media info"_hash:
      return "SELECT"
             " img_media_type AS 'Media Type',"
             " img_width AS 'Width',"
             " img_height AS 'Height'"
             " FROM image"
             " WHERE img_name = ?";

    case "image data from timestamp and name"_hash:
    case "image data from timestamp and name by width"_hash:
      return "SELECT"
             " img_width AS 'Width',"
             " img_height AS 'Height',"
             " img_data AS 'Data'"
             " FROM image_data"
             " WHERE img_timestamp = ?"
             " AND img_name = ?"
             " ORDER BY img_width";

    case "image data from timestamp and name by height"_hash:
      return "SELECT"
             " img_width AS 'Width',"
             " img_height AS 'Height',"
             " img_data AS 'Data'"
             " FROM image_data"
             " WHERE img_timestamp = ?"
             " AND img_name = ?"
             " ORDER BY img_height";

    case "image data from name"_hash:
    case "image data from name sort by width"_hash:
      return "SELECT"
             " img_width AS 'Width',"
             " img_height AS 'Height',"
             " img_data AS 'Data'"
             " FROM image_data"
             " WHERE img_name = ?"
             " ORDER BY img_width";

    case "image data from name sort by height"_hash:
      return "SELECT"
             " img_width AS 'Width',"
             " img_height AS 'Height',"
             " img_data AS 'Data'"
             " FROM image_data"
             " WHERE img_name = ?"
             " ORDER BY img_height";
  }
  return "";
}
