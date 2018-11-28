#include "codeline.h"
/***************************************************************************
 *3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
 * Class 'CodeLine' as a container for one line of code.
 *
 * Author/copyright:  Duncan A. Buell.  All rights reserved.
 * Used with permission and modified by: Tsung Wei Wu
 * Date: 20 November 2018
**/

/***************************************************************************
 * Constructor
**/
CodeLine::CodeLine() {
}
int pc = 0;
int linecounter = 0;
CodeLine::CodeLine(string codeline) {
  string mnemonic = "";
  string label = "";
  string addr = "";
  string symoperand = "";
  string hexoperand = "";
  string comments = "";
  string code = "1100110011001100";
  if (codeline.length() < 21) {
    codeline.append(21-codeline.length(), ' '); //pad to length 21, for easier code
  }

  if (codeline.substr(0,1)=="*") {
    SetCommentsOnly(linecounter,codeline);
    linecounter++;
  } else {
    if (codeline.substr(0,3) != "   ") {
      label = codeline.substr(0,3);
    } //if field is empty, value of label_ will be null
    mnemonic = codeline.substr(4,3);
    addr = codeline.substr(8,1);
    if (codeline.substr(10,3) != "   ") {
      symoperand = codeline.substr(10,3);
    } //if field is empty, value of symoperand_ will be null
    if (codeline.substr(14,5) != "     ") {
      hexoperand = codeline.substr(14,5);
    }
    if (codeline.substr(20,1) == "*") {
      comments = codeline.substr(20);
    }
    SetCodeLine(linecounter, pc, label, mnemonic, addr, symoperand, hexoperand, comments, code);
    pc++;
    linecounter++;
  }
}
//CodeLine::CodeLine(Globals globals) {
//  globals_ = globals;
//}

/***************************************************************************
 * Destructor
**/
CodeLine::~CodeLine() {
}

/***************************************************************************
 * Accessors and Mutators
**/

/***************************************************************************
 * Accessor for the 'addr_'.
**/
string CodeLine::GetAddr() const {
  return addr_;
}

/***************************************************************************
 * Accessor for the 'code_'.
**/
string CodeLine::GetCode() const {
  return code_;
}

/***************************************************************************
 * Accessor for the 'comments_'.
**/
string CodeLine::GetComments() const {
  return comments_;
}

/***************************************************************************
 * Accessor for the 'error_messages_'.
**/
string CodeLine::GetErrorMessages() const {
  return error_messages_;
}

/***************************************************************************
 * Accessor for the 'hex_'.
**/
Hex CodeLine::GetHexObject() const {
  return hex_;
}

/***************************************************************************
 * Accessor for the 'label_'.
**/
string CodeLine::GetLabel() const {
  return label_;
}

/***************************************************************************
 * Accessor for the 'mnemonic_'.
**/
string CodeLine::GetMnemonic() const {
  return mnemonic_;
}

/***************************************************************************
 * Accessor for the 'symoperand_'.
**/
string CodeLine::GetSymOperand() const {
  return symoperand_;
}

/***************************************************************************
 * Boolean indicator of the presence of a label.
**/
bool CodeLine::HasLabel() const {
  bool returnValue = !(label_.empty() || label_ == "   ");
  return returnValue;
}

/***************************************************************************
 * Boolean indicator of the presence of a symbolic operand.
**/
bool CodeLine::HasSymOperand() const {
  bool returnValue = !(symoperand_.empty() || symoperand_ == "   ");
  return returnValue;
}

/***************************************************************************
 * Accessor for 'is_all_comment_'.
**/
bool CodeLine::IsAllComment() const {
  return is_all_comment_;
}

/***************************************************************************
 * General functions.
**/

/***************************************************************************
 * Function 'SetCodeLine'.
 * Sets the values for a line of code that isn't all comments.
 *
 * Parameters:
 *   linecounter - the line counter for this line in source
 *   pc - the program counter if it is there
 *   label - the label if it is there
 *   mnemonic - the mnemonic of the opcode
 *   addr - the direct/indirect addresssing asterisk
 *   symoperand - the symbolic operand if it is there
 *   hexoperand - the hexoperand if it is there
 *   comments - comments if any
**/
void CodeLine::SetCodeLine(int linecounter, int pc, string label,
                           string mnemonic, string addr, string symoperand,
                           string hexoperand, string comments, string code) {
  linecounter_ = linecounter;
  pc_ = pc;
  label_ = label;
  mnemonic_ = mnemonic;
  addr_ = addr;
  symoperand_ = symoperand;
  hex_ = Hex(hexoperand);
  comments_ = comments;
  code_ = code;

  is_all_comment_ = false;
}

/***************************************************************************
 * Function 'SetCommentsOnly'.
 * Sets a line of code that is all comments.
 *
 * Parameters:
 *   line - the code line that is taken to be all comments
**/
void CodeLine::SetCommentsOnly(int linecounter, string line) {
  comments_ = line; // assume asterisk was not stripped already
  is_all_comment_ = true;
  linecounter_ = linecounter;
}

/***************************************************************************
 * Function 'SetErrorMessages'.
 * Sets the 'error_messages_' variable for later printing.
 *
 * Parameters:
 *   messages - the string of messages
**/
void CodeLine::SetErrorMessages(string messages) {
}

/***************************************************************************
 * Function 'SetMachineCode'.
 * Set the 'code_' for this line of code.
 *
 * Parameters:
 *   code - the code to set
**/
void CodeLine::SetMachineCode(string code) {
  code_ = code;
}

/***************************************************************************
 * Function 'SetPC'.
 * Set the PC for this line of code.
 *
 * Parameters:
 *   what - the value to set as the PC
**/
void CodeLine::SetPC(int what) {
}

/***************************************************************************
 * Function 'ToString'.
 * This function formats a 'CodeLine' for prettyprinting.
 *
 * Returns:
 *   the prettyprint string for printing
**/
string CodeLine::ToString() const {
#ifdef EBUG
  Utils::log_stream << "enter ToString" << endl;
#endif
  string s = "";


  s += Utils::Format(linecounter_, 5) + " ";

  s += Utils::Format(pc_, 4) + "  ";
  s += DABnamespace::DecToBitString(pc_, 12) + " ";

  if (code_ == "nullcode") {
    s += Utils::Format("xxxx xxxx xxxx xxxx", 19);
  } else {
    s += Utils::Format("1100 1100 1100 1100", 19);
    
  }

  if (label_ == "nulllabel") {
    s += " " + Utils::Format("...", 3);
  } else {
    s += " " + Utils::Format(label_, 3);
  }

  if (mnemonic_ == "nullmnemonic") {
    s += " " + Utils::Format("...", 3);
  } else {
    s += " " + Utils::Format(mnemonic_, 6);
  }

  if (addr_ == "*") {
    s += " " + Utils::Format("*", 1);
  } else {
    s += " " + Utils::Format(" ", 1);
  }

  if (symoperand_ == "nullsymoperand") {
    s += " " + Utils::Format("...", 3);
  } else {
    s += " " + Utils::Format(symoperand_, 3);
  }

  if (hex_.IsNull()) {
    s += " " + Utils::Format(".....", 5);
  } else {
    s += " " + hex_.ToString();
  }

  if (comments_ != "nullcomments") {
    if (is_all_comment_) {
      s = Utils::Format(linecounter_, 5) + " ";
      s += Utils::Format(" ", 41);
      s += " " + comments_;
    } else {
      s += " " + comments_;
    }
  }

  if ((error_messages_.length() > 0) &&
      (error_messages_ != "nullerrormessages")) {
    s += error_messages_;
  }

#ifdef EBUG
  Utils::log_stream << "leave ToString" << endl;
#endif
  return s;
}
