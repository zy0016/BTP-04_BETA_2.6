/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : MailInterface.c
*
* Purpose  : 
*
\**************************************************************************/

//button character
#define IDP_MAIL_BOOTEN_OK              ML("OK")
#define IDP_MAIL_BUTTON_CANCEL          ML("Cancel")
#define IDP_MAIL_BUTTON_YES             ML("Yes")
#define IDP_MAIL_BUTTON_NO              ML("No")
#define IDP_MAIL_BUTTON_BACK            ML("Back")
#define IDP_MAIL_BUTTON_ADD             ML("Add")
#define IDP_MAIL_BUTTON_ADDRECIPIENT    ML("Add recipient")
#define IDP_MAIL_BUTTON_SAVE            ML("Save")
#define IDP_MAIL_BUTTON_SAVEDRAFTS      ML("Save to Drafts")
#define IDP_MAIL_BUTTON_SEND            ML("Send")
#define IDP_MAIL_BUTTON_SENDOPTION      ML("Send with options")
#define IDP_MAIL_BUTTON_INSERT          ML("Insert")
#define IDP_MAIL_BUTTON_ATTACHMENT      ML("Attachments")
#define IDP_MAIL_BUTTON_INFO            ML("Details")
#define IDP_MAIL_BUTTON_EDIT            ML("Edit")
#define IDP_MAIL_BUTTON_ACTI            ML("gary activate")
#define IDP_MAIL_BUTTON_DELETE          ML("Delete")
#define IDP_MAIL_BUTTON_REMOVE          ML("Remove")

#define IDP_MAIL_BUTTON_OPEN            ML("Open")
#define IDP_MAIL_BUTTON_REPLY           ML("Reply")
#define IDP_MAIL_BUTTON_REPLYSUB        ML("Reply")
#define IDP_MAIL_BUTTON_REPLYTO         ML("To sender")
#define IDP_MAIL_BUTTON_REPLYALL        ML("To all")
#define IDP_MAIL_BUTTON_FORWARD         ML("Forward")
#define IDP_MAIL_BUTTON_MOVE            ML("Copy")
#define IDP_MAIL_BUTTON_MOVETOFOLDER    ML("Copy to folder")
#define IDP_MAIL_BUTTON_MOVEMANY        ML("Copy many")
#define IDP_MAIL_BUTTON_SELECT          ML("Select")
#define IDP_MAIL_BUTTON_ALL             ML("All")
#define IDP_MAIL_BUTTON_CONNECT         ML("Connect")
#define IDP_MAIL_BUTTON_DISCONNECT      ML("Disconnect")
#define IDP_MAIL_BUTTON_RETRIEVE        ML("Retrieve e-mail")
#define IDP_MAIL_BUTTON_MARK            ML("Mark as read")
#define IDP_MAIL_BUTTON_DELETEMANY      ML("Delete many")
#define IDP_MAIL_BUTTON_PLAY            ML("Play")
#define IDP_MAIL_BUTTON_STOP            ML("Stop")

#define IDP_MAIL_BUTTON_PICTURE         ML("Picture")
#define IDP_MAIL_BUTTON_SOUND           ML("Sound")
#define IDP_MAIL_BUTTON_NOTE            ML("Note")
#define IDP_MAIL_BUTTON_TEMPLATE        ML("Template")

#define IDP_MAIL_VIEW_GOTOURL               ML("Go to URL")
#define IDP_MAIL_VIEW_ADDTOBM               ML("Add to bookmarks")
#define IDP_MAIL_VIEW_VIEWONMAP             ML("View on map")
#define IDP_MAIL_VIEW_SETASDESTINATION      ML("Set as destination")
#define IDP_MAIL_VIEW_SAVEASWAYPOINT        ML("Save as waypoint")
#define IDP_MAIL_VIEW_SEND                  ML("Write message")
#define IDP_MAIL_VIEW_SEND_SMS              ML("SMS")
#define IDP_MAIL_VIEW_SEND_MMS              ML("MMS")
#define IDP_MAIL_VIEW_SEND_EMAIL            ML("E-mail")
#define IDP_MAIL_VIEW_ATTACHMENTS           ML("Attachments")
#define IDP_MAIL_VIEW_REPLY                 ML("Reply")
#define IDP_MAIL_VIEW_REPLYMANY             ML("Reply")
#define IDP_MAIL_VIEW_FORWARD               ML("Forward")
#define IDP_MAIL_VIEW_ADDTOCONTACTS         ML("Save to Contacts")
#define IDP_MAIL_VIEW_ADDTOCONTACTS_UPDATE  ML("Add to contact")
#define IDP_MAIL_VIEW_ADDTOCONTACTS_CREATE  ML("New contact")
#define IDP_MAIL_VIEW_FIND                  ML("Find")
#define IDP_MAIL_VIEW_FIND_PHONE            ML("Phone number")
#define IDP_MAIL_VIEW_FIND_EMAIL            ML("E-mail address")
#define IDP_MAIL_VIEW_FIND_WEB              ML("Web address")
#define IDP_MAIL_VIEW_FIND_COORDINATE       ML("Coordinates")
#define IDP_MAIL_VIEW_HIDEFOUNDITEMS        ML("Hide found items")
#define IDP_MAIL_VIEW_MOVETOFOLDER          ML("Move to folder")
#define IDP_MAIL_VIEW_MESSAGEINFO           ML("Details")
#define IDP_MAIL_VIEW_DELETE                ML("Delete")

//string character
#define IDP_MAIL_STRING_EMAIL           ML("Email")
#define IDP_MAIL_STRING_NOSPACE         ML("No enough space")
#define IDP_MAIL_STRING_WARNING         ML("Warning")
#define IDP_MAIL_STRING_OPENERROR       ML("Message cannot be opened")
#define IDP_MAIL_STRING_DIALREJECT      ML("Dial is rejected")
#define IDP_MAIL_STRING_INSUFDIAL       ML("Insufficient dial resource")
#define IDP_MAIL_STRING_MAILADDRERR     ML("Mail address error")
#define IDP_MAIL_STRING_MAXMAILSIZE     ML("E-mail too large.\nPlease remove\nattachments.")
#define IDP_MAIL_STRING_MAXTEXTSIZE     ML("E-mail too large.\nPlease remove text.")
#define IDP_MAIL_STRING_MESNOTSAVE      ML("Not enough memory.Message not saved.")
#define IDP_MAIL_STRING_MAXMAILBOX      ML("Max mailbox number\nreached")
#define IDP_MAIL_STRING_MAILALDEL       ML("E-mail already deleted")
#define IDP_MAIL_STRING_UNSUPPORTMAIL   ML("Unsupport file type.")

#define IDP_MAIL_STRING_ATTINSERTED     ML("Attachment inserted")
#define IDP_MAIL_STRING_INSERTFAILED    ML("Inserting failed")

#define IDP_MAIL_STRING_SENDRECIERR     ML("Invalid phone number.Sending failed.")
#define IDP_MAIL_STRING_SENDNONET       ML("No network.Sending failed.")
#define IDP_MAIL_STRING_SENDFAIL        ML("Sending failed.")
#define IDP_MAIL_STRING_SENDING         ML("Sending...")
#define IDP_MAIL_STRING_SENDSUCC        ML("Message sent")//ML("Send mail succeed")

#define IDP_MAIL_STRING_TIMEOUT         ML("Timeout")
#define IDP_MAIL_STRING_NONET           ML("No network")
#define IDP_MAIL_STRING_NOCONNECT       ML("No connections")
#define IDP_MAIL_STRING_NETBROKEN       ML("Network broken!")
#define IDP_MAIL_STRING_NOSIM           ML("Sending failed. No SIM card")
#define IDP_MAIL_STRING_NOSIMCON        ML("Connection failed. No SIM card")
#define IDP_MAIL_STRING_TOOLARGE        ML("E-mail is too large")
//#define IDP_MAIL_STRING_NOMAILBOX       ML("Please set up\nmailbox for E-mail")
#define IDP_MAIL_STRING_INVALIDMAILSET  ML("Invalid mailbox settings")
#define IDP_MAIL_STRING_NORETRMAIL      ML("No retrieved mail")

#define IDP_MAIL_STRING_CONNECTED       ML("Connected")
#define IDP_MAIL_STRING_DISCONNECTED    ML("Disconnected")
#define IDP_MAIL_STRING_CONNECTING      ML("Connecting...")
#define IDP_MAIL_STRING_RETRIEVING      ML("Retrieving...")
#define IDP_MAIL_STRING_DOWNLOADING     ML("Downloading...")
#define IDP_MAIL_STRING_DISCONNECTING   ML("Disconnecting...")
#define IDP_MAIL_STRING_RETRIEVED       ML("E-mail retrieved")
#define IDP_MAIL_STRING_CONNECTFAL      ML("Connection failed")
#define IDP_MAIL_STRING_DISCONFAL       ML("Disconnect failed")
#define IDP_MAIL_STRING_RETRIEVRAL      ML("Retrieved failed")
#define IDP_MAIL_STRING_RETRNOW         ML("E-mail is not\nretrieved.\nRetrieve now?")
#define IDP_MAIL_STRING_CONNECT         ML("Connect?")
#define IDP_MAIL_STRING_DISCONNECTTO    ML("Disconnect?")
#define IDP_MAIL_STRING_NOEMAILS        ML("No e-mails")
#define IDP_MAIL_STRING_NOMAILSET       ML("No mailbox set")

#define IDP_MAIL_STRING_REMOVEJUD       ML("Remove?")
#define IDP_MAIL_STRING_REMOVED         ML("Removed")
#define IDP_MAIL_STRING_REMOVEFAIL      ML("Remove fail")

#define IDP_MAIL_STRING_CANNOTMOVE      ML("Cannot move unretrieved messages")
#define IDP_MAIL_STRING_MOVEFAIL        ML("Move failed")
#define IDP_MAIL_STRING_MSGMOVED        ML("Message moved")
#define IDP_MAIL_STRING_MSGSMOVED       ML("Messages moved")

#define IDP_MAIL_STRING_DRAFTFULL       ML("Drafts full.Message not saved.")
#define IDP_MAIL_STRING_RECERROR        ML("Invalid\nvalue")
#define IDP_MAIL_STRING_SAVEDTODRAFT    ML("Saved to drafts")
#define IDP_MAIL_STRING_SAVETODRAFT     ML("Save to Drafts?")
#define IDP_MAIL_STRING_SAVEDFAIL       ML("Save failed")
#define IDP_MAIL_STRING_SAVED           ML("Saved")
#define IDP_MAIL_STRING_SAVINGFAIL      ML("Saving failed")

#define IDP_MAIL_STRING_DEFINENAME      ML("Please define name")
#define IDP_MAIL_STRING_ERRORNAME       ML("Please define another name")
#define IDP_MAIL_STRING_DEFINELOGIN     ML("Please define login")
#define IDP_MAIL_STRING_DEFINESIGN      ML("Please define signature")
#define IDP_MAIL_STRING_DEFINEADDR      ML("Please define\ne-mail address")
#define IDP_MAIL_STRING_DEFINEOUTGO     ML("Please define\noutgoing mail server")
#define IDP_MAIL_STRING_DEFINEINCOM     ML("Please define\nincoming mail server")
#define IDP_MAIL_STRING_NOMAILBOX       ML("Please set up\nmailbox for E-mail")
#define IDP_MAIL_STRING_SELMESSAGE      ML("Please select messages")
#define IDP_MAIL_STRING_MODMAILTYPE     ML("Cannot modify selected mailbox type. please define new mailbox")

#define IDP_MAIL_STRING_MAILBOXINUSE    ML("Mailbox in use:\n")
#define IDP_MAIL_STRING_DELETE          ML("\nDelete?")
#define IDP_MAIL_STRING_DELETEMSG       ML("Delete message?")
#define IDP_MAIL_STRING_DELSEL          ML("Delete selected messages?")
#define IDP_MAIL_STRING_DELALL          ML("Delete all?")
#define IDP_MAIL_STRING_ACTIVATED       ML("\nActivated")
#define IDP_MAIL_STRING_DELETEACTI      ML("Active.\nDelete anyway?")
#define IDP_MAIL_STRING_DELETED         ML("Deleted")
#define IDP_MAIL_STRING_DELFAIL         ML("Delete fail")
#define IDP_MAIL_STRING_CONFIRM         ML("Confirm")
#define IDP_MAIL_STRING_DELSERVER       ML("Delete also e-mail in server?")
#define IDP_MAIL_STRING_DELSERVERS      ML("Delete also e-mails in server?")
#define IDP_MAIL_STRING_MISSDATA        ML("All required data\nnot set.\nSet now?")

#define IDP_MAIL_STRING_RECIERROR       ML("Recipient error")
#define IDP_MAIL_STRING_DELETEENGROSS   ML("Mailbox working please delete again a few minutes later")
#define IDP_MAIL_STRING_MODIFYENGROSS   ML("Mailbox working please modify again a few minutes later")

//title character
#define IDP_MAIL_TITLE_TYPE             ML("Type:")
#define IDP_MAIL_TITLE_EMAIL            ML("E-mail")
#define IDP_MAIL_TITLE_FROM             ML("From:")
#define IDP_MAIL_TITLE_TO               ML("To:")
#define IDP_MAIL_TITLE_CC               ML("Cc:")
#define IDP_MAIL_TITLE_DATE             ML("Date:")
#define IDP_MAIL_TITLE_TIME             ML("Time:")
#define IDP_MAIL_TITLE_SIZE             ML("Size:")
#define IDP_MAIL_TITLE_SUBJECT          ML("Subject:")
#define IDP_MAIL_TITLE_MSGINFO          ML("Details")
#define IDP_MAIL_TITLE_NEWMAILBOX       ML("New mailbox")

#define IDP_MAIL_TITLE_MAILBOX          ML("Mailbox")
#define IDP_MAIL_TITLE_MAILNAME         ML("Mailbox name")
#define IDP_MAIL_TITLE_MAILNAMECOLON    ML("Mailbox name:")
#define IDP_MAIL_TITLE_CONNECT          ML("Connection")
#define IDP_MAIL_TITLE_MAILADDR         ML("E-mail address")
#define IDP_MAIL_TITLE_MAILADDRCOLON    ML("E-mail address:")
#define IDP_MAIL_TITLE_OUTSERVER        ML("Outgoing mail server")
#define IDP_MAIL_TITLE_OUTSERVERCOLON   ML("Outgoing mail server:")
#define IDP_MAIL_TITLE_SENDMSG          ML("Message sending")
#define IDP_MAIL_TITLE_CCTOSELF         ML("CC to self")
#define IDP_MAIL_TITLE_SIGNATURE        ML("Signature")
#define IDP_MAIL_TITLE_SIGNATURECOLON   ML("Signature:")
#define IDP_MAIL_TITLE_LOGIN            ML("Login")
#define IDP_MAIL_TITLE_LOGINCOLON       ML("Login:")
#define IDP_MAIL_TITLE_PASSWORD         ML("Password")
#define IDP_MAIL_TITLE_PASSWORDCOLON    ML("Password:")
#define IDP_MAIL_TITLE_INSERVER         ML("Incoming mail server")
#define IDP_MAIL_TITLE_INSERVERCOLON    ML("Incoming mail server:")
#define IDP_MAIL_TITLE_MAILTYPE         ML("Mailbox type")
#define IDP_MAIL_TITLE_AUTORET          ML("Autoretrieval")
#define IDP_MAIL_TITLE_SECURITY         ML("Security")
#define IDP_MAIL_TITLE_SECURELOGIN      ML("Secure login")
#define IDP_MAIL_TITLE_RETRATT          ML("Retrieve attachments")
#define IDP_MAIL_TITLE_RETRHEAD         ML("Retrieve headers")
//value
#define IDP_MAIL_VALUE_REQUIRED         ML("Required")
#define IDP_MAIL_VALUE_CONNECTAUTO      ML("Connect automatically")
#define IDP_MAIL_VALUE_WHENCONNECT      ML("When connected")
#define IDP_MAIL_VALUE_POP3             ML("POP3")
#define IDP_MAIL_VALUE_IMAP4            ML("IMAP4")
#define IDP_MAIL_VALUE_ON               ML("On")
#define IDP_MAIL_VALUE_OFF              ML("Off")
#define IDP_MAIL_VALUE_SSL              ML("SSL")
#define IDP_MAIL_VALUE_10MIN            ML("10 min")
#define IDP_MAIL_VALUE_30MIN            ML("30 min")
#define IDP_MAIL_VALUE_1H               ML("1 h")
#define IDP_MAIL_VALUE_2H               ML("2 h")
#define IDP_MAIL_VALUE_6H               ML("6 h")
#define IDP_MAIL_VALUE_12H              ML("12 h")
#define IDP_MAIL_VALUE_24H              ML("24 h")
#define IDP_MAIL_VALUE_ALL              ML("All")
