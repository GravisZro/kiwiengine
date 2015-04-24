#include "wikieditor.h"

#include <htmlgen/encode.h>

void CWikiEditor::extractSection(const HUString& header, CCharStream& stream)
{
  while(!(++stream).atEnd() &&
        (stream        != ECharacter::endline ||
         stream.next() != ECharacter::equals_sign))
  {
    m_sectiontext.append(stream);
  }
}

void CWikiEditor::clear(void)
{
  m_preview = false;
  m_enabled = false;
  m_section.clear();
  m_sectiontext.clear();
}

void CWikiEditor::render(void)
{
  if(!m_enabled || !hasParent() || m_sectiontext.isEmpty())
    return;

  appendElement("br");
  appendElement("div", {"id", "toolbar"});
  becomeChild("form", { {"id", "editform"},
                        {"name", "editform"},
                        {"method", "post"},
                        {"action", encode_link(m_title) +
                          (m_section.isEmpty() ? "?action=submit" :
                                                ("?action=submit&amp;section=" + encoder(m_section)))
                        },
                        {"enctype", "multipart/form-data"} });

    becomeChild("textarea", { {"tabindex", "1"},
                              {"accesskey", ","},
                              {"id", "wpTextbox1"},
                              {"cols", "80"},
                              {"rows", "25"},
                              {"style", ""},
                              {"lang", "en"},
                              {"dir", "ltr"},
                              {"name", "wpTextbox1"} });
      appendText(m_sectiontext);
    becomeParent();

    becomeChild("span", {{"id", "wpSummaryLabel"}, {"class", "mw-summary"}});
      becomeChild("label", {"for", "wpSummary"});
        appendText("Summary");
      becomeParent();
    becomeParent();

    appendElement("input", {{"id", "wpSummary"},
                            {"name", "wpSummary"},
                            {"class", "mw-summary"},
                            {"maxlength", "200"},
                            {"tabindex", "1"},
                            {"size", "60"},
                            {"spellcheck", "true"},
                            {"title", "Enter a short summary [b]"},
                            {"accesskey", "b"}});


    becomeChild("div", {"class", "editCheckboxes"});
      appendElement("input", {{"id", "wpMinoredit"},
                              {"name", "wpMinoredit"},
                              {"type", "checkbox"},
                              {"value", "1"},
                              {"tabindex", "3"},
                              {"accesskey", "i"}});
      appendText("&#160;");
      becomeChild("label", {{"id", "mw-editpage-minoredit"},
                            {"for", "wpMinoredit"},
                            {"title", "Mark this as a minor edit [i]"}});
        appendText("This is a minor edit");
      becomeParent();

      appendElement("input", {{"id", "wpWatchthis"},
                              {"name", "wpWatchthis"},
                              {"type", "checkbox"},
                              {"value", "1"},
                              {"tabindex", "4"},
                              {"accesskey", "w"}});
      appendText("&#160;");
      becomeChild("label", {{"id", "mw-editpage-watch"},
                            {"for", "wpWatchthis"},
                            {"title", "Add this page to your watchlist [w]"}});
        appendText("Watch this page");
      becomeParent();
    becomeParent();

    appendElement("div", {"id", "editpage-copywarn"});

    becomeChild("div", {"class", "editButtons"});
      appendElement("input", {{"id", "wpSave"   }, {"type", "submit"}, {"tabindex", "5"}, {"accesskey", "s"}, {"value", "Save page"   }, {"title", "Save your changes [s]"}});
      appendElement("input", {{"id", "wpPreview"}, {"type", "submit"}, {"tabindex", "6"}, {"accesskey", "p"}, {"value", "Show preview"}, {"title", "Preview your changes, please use this before saving! [p]"}});
      appendElement("input", {{"id", "wpDiff"   }, {"type", "submit"}, {"tabindex", "7"}, {"accesskey", "v"}, {"value", "Show changes"}, {"title", "Show which changes you made to the text [v]"}});

      becomeChild("span", {"class", "cancelLink"});
        becomeChild("a", {{"id", "mw-editform-cancel"}, {"href", encode_link(m_title)}, {"title", m_title}});
          appendText("Cancel");
        becomeParent();
      becomeParent();

      becomeChild("span", {"class", "mw-editButtons-pipe-separator"});
        appendText(" | ");
      becomeParent();

      becomeChild("span", {"class", "editHelp"});
        becomeChild("a", {{"target", "helpwindow"}, {"href", "/Help:Editing"}});
          appendText("Editing help");
        becomeParent();
        appendText(" (opens in new window)");
      becomeParent();
    becomeParent();

  becomeParent();
}
