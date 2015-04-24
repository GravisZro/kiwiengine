#include "allowed_tags.h"

bool bbcode::dealias(tag_t& tag)
{
  HUString name = tag.tag_name;

  if(name.at(0) == '/')
    name = name.mid(1);
/*
  if(tag.tag_name == "pre")
    name = "code";
  else
*/
  if(tag.tag_name == "hr")
    name = "rule";
  else if(tag.tag_name == "br")
    name = "break";
  else if(tag.tag_name == "item")
    name = "*";
  else if(tag.tag_name == "image")
    name = "img";
  else if(tag.tag_name == "link")
    name = "url";
  else if(tag.tag_name == "line")
    name = "rule";
  else if(tag.tag_name == "bgcolor")
    name = "highlight";
  else if(tag.tag_name == "mono")
  {
    name = "font";
    tag.attributes.clear();
    tag.attributes.emplace_back("class", "wiki_mono");
  }
  else if(tag.tag_name == "cursive")
  {
    name = "font";
    tag.attributes.clear();
    tag.attributes.emplace_back("class", "wiki_cursive");
  }
  else if(tag.tag_name == "h1" ||
          name == "h2" ||
          name == "h3" ||
          name == "h4" ||
          name == "h5" ||
          name == "h6")
  {
    tag.attributes.clear();
    tag.attributes.emplace_back("", name);
    name = "size";
  }
  else
  {
    return false; // not aliased
  }

  if(tag.tag_name.at(0) == '/')
  {
    tag.attributes.clear();
  }

  tag.tag_name = name;
  return true;
}


bool bbcode::romanToInt(const QString& input, int& value)
{
  if(input != input.toUpper() || input != input.toLower()) // must be all uppercase or all lowercase
    return false;

  QString str = input.toUpper();
  const QMap<QChar, int> roman = { { 'M', 1000 }, { 'D', 500 }, { 'C', 100 }, { 'L', 50 }, { 'X', 10 }, { 'I', 1 }};
  const int end = str.size() - 1;
  value = 0;

  for (int pos = 0; pos < end; ++pos)
  {
    if(!roman.contains(str.at(pos)))
      return false; // bad input
    if(roman[str[pos]] < roman[str[pos+1]])
      value -= roman[str[pos]];
    else
      value += roman[str[pos]];
  }
  value += roman[str[end]];
  return true;
}

bool bbcode::lettersToInt(const QString& input, int& value)
{
  if(input != input.toUpper() || input != input.toLower()) // must be all uppercase or all lowercase
    return false;

  QString str = input.toUpper();
  value = 0;
  for (int pos = 0; pos < str.size(); ++pos)
  {
    if(!str[pos].isLetter())
      return false; // bad input
    value += str[pos].toLatin1() - 'A' + 1;
    value *= 26;
  }

  return true;
}


bool bbcode::closeTag (tag_t& tag, CElement& element)
{
  Q_ASSERT(tag.tag_name.at(0) == '/'); // this function is only for closing tags!

  tag.tag_name = tag.tag_name.mid(1).toLower();

  if(simple_pairs.contains(tag.tag_name) || // if it's a simple tag
     table_only.contains(tag.tag_name)) // or a table tag
  {
    if(!element.inStack(tag.tag_name)) // if tag isnt actually open
      return false; // extra closing tag! ABORT!

    while(element.nodeName() != tag.tag_name) // while not close tag
      element.becomeParent(); // close child tag

    element.becomeParent(); // close target tag
    return true;
  }

  auto entry = complex_tags.find(tag.tag_name);
  if(entry != complex_tags.end()) // if it's a complex tag
  {
    QStringList list = entry.value();//->second;
    int min_depth = INT_MAX;
    auto min_pos = list.end();
    for(auto pos = list.begin(); pos != list.end(); ++pos)
    {
      QDomNode node = element;
      int depth = 0;
      while(node.nodeName() != *pos && node.isElement())
      {
        ++depth;
        node = node.parentNode();
      }
      if(depth < min_depth &&
         node.isElement() &&
         node.nodeName() == *pos)
      {
        min_depth = depth;
        min_pos = pos;
      }
    }

    if(min_pos != list.end())
    {
      do { element.becomeParent(); } while(min_depth--);
      return true;
    }
  }

  return false; // unknown tag type!
}
