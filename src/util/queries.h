#ifndef QUERIES_H
#define QUERIES_H

#include <QMetaType>
#include <QMap>
#include <QString>

#include <soloutils>


enum class ENamespaces
{
  eArticle = 0,
  eArticleTalk,
  eUser,
  eUserTalk,
  eFile,
  eFileTalk,
  eMediaWiki,
  eMediaWikiTalk,
  eTemplate,
  eTemplateTalk,
  eHelp,
  eHelpTalk,
  eCategory,
  eCategoryTalk,

  ePortal = 100,
  ePortalTalk,

  eBook = 108,
  eBookTalk,

  eDraft = 118,
  eDraftTalk,

  eEducationProgram = 446,
  eEducationProgramTalk,

  eTimedText = 710,
  eTimedTextTalk,

  eModule = 828,
  eModuleTalk,

  eTopic = 2600,

// virtual namespaces
  eSpecial = -1,
  eMedia  = -2,

  BADVALUE
};

const QMap<QString, ENamespaces> nsmap =
{
  { "Article"               , ENamespaces::eArticle              },
  { "Article talk"          , ENamespaces::eArticleTalk          },

  { "User"                  , ENamespaces::eUser                 },
  { "User talk"             , ENamespaces::eUserTalk             },

  { "File"                  , ENamespaces::eFile                 },
  { "File talk"             , ENamespaces::eFileTalk             },

  { "MediaWiki"             , ENamespaces::eMediaWiki            },
  { "MediaWiki talk"        , ENamespaces::eMediaWikiTalk        },

  { "Template"              , ENamespaces::eTemplate             },
  { "Template talk"         , ENamespaces::eTemplateTalk         },

  { "Help"                  , ENamespaces::eHelp                 },
  { "Help talk"             , ENamespaces::eHelpTalk             },

  { "Category"              , ENamespaces::eCategory             },
  { "Category talk"         , ENamespaces::eCategoryTalk         },

  { "Portal"                , ENamespaces::ePortal               },
  { "Portal talk"           , ENamespaces::ePortalTalk           },

  { "Book"                  , ENamespaces::eBook                 },
  { "Book talk"             , ENamespaces::eBookTalk             },

  { "Draft"                 , ENamespaces::eDraft                },
  { "Draft talk"            , ENamespaces::eDraftTalk            },

  { "Education program"     , ENamespaces::eEducationProgram     },
  { "Education program talk", ENamespaces::eEducationProgramTalk },

  { "Timed text"            , ENamespaces::eTimedText            },
  { "Timed text talk"       , ENamespaces::eTimedTextTalk        },

  { "Module"                , ENamespaces::eModule               },
  { "Module talk"           , ENamespaces::eModuleTalk           },

  { "Topic"                 , ENamespaces::eTopic                },

  { "Special"               , ENamespaces::eSpecial              },
  { "Media "                , ENamespaces::eMedia                },
};

Q_DECLARE_METATYPE(ENamespaces)

const char* query(const char* name);

#endif // QUERIES_H
