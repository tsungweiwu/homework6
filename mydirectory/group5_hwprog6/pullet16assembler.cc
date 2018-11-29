#include "pullet16assembler.h"

/***************************************************************************
 *3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456
 * Class 'Assembler' for assembling code.
 *
 * Author/copyright:  Duncan A. Buell.  All rights reserved.
 * Used with permission and modified by: Tsung Wei Wu
 * Date: 20 November 2018
**/

/***************************************************************************
 * Constructor
**/
Assembler::Assembler() {
}
// trying GitHub
/***************************************************************************
 * Destructor
**/
Assembler::~Assembler() {
}

/***************************************************************************
 * Accessors and Mutators
**/

/***************************************************************************
 * General functions.
**/

/***************************************************************************
 * Function 'Assemble'.
 * This top level function assembles the code.
 *
 * Parameters:
 *   in_scanner - the scanner to read for source code
 *   out_stream - the output stream to write to
**/
void Assembler::Assemble(Scanner& in_scanner, string binary_filename,
                         ofstream& out_stream) {
#ifdef EBUG
  Utils::log_stream << "enter Assemble" << endl;
#endif

// Reading Ascii input
  int linecounter_ = 0;
  CodeLine codeline;
  while (linecounter_ < 4096) {
    string line = in_scanner.NextLine();
    if (line == "") { break; }
    codeline = CodeLine();

    string mnemonic = "nullmnemonic";
    string label = "nulllabel";
    string addr = "";
    string symoperand = "nullsymoperand";
    string hexoperand = "";
    string comments = "nullcomments";
    string code = "nullcode";
    if (line.length() < 21) {
      line.append(21-line.length(), ' '); //pad to length 21, for easier code
    }

    if (line.substr(0,1)=="*") {
      codeline.SetCommentsOnly(linecounter_,line);
    } else {
      if (line.substr(0,3) != "   ") {
        label = line.substr(0,3);
      } //if field is empty, value of label_ will be null
      mnemonic = line.substr(4,3);
      addr = line.substr(8,1);
      if (line.substr(10,3) != "   ") {
        symoperand = line.substr(10,3);
      } //if field is empty, value of symoperand_ will be null
      if (line.substr(14,5) != "     ") {
        hexoperand = line.substr(14,5);
      }
      if (line.substr(20,1) == "*") {
        comments = line.substr(20);
      }
      codeline.SetCodeLine(linecounter_, 0, label, mnemonic, addr, symoperand, hexoperand, comments, code);
    }
    codelines_.push_back(codeline);
    if (codeline.GetMnemonic() == "END") {
      found_end_statement_ = true;
      break;
    }
    linecounter_++;
  }

// Reading Binary File
  vector<string> input_read_back;
  std::ifstream input(binary_filename, std::ifstream::binary);

  if (input) {
    input.seekg(0, input.end);
    int length = input.tellg();
    input.seekg(0, input.beg);
    Utils::log_stream << "BINARY LENGTH, WORDCOUNT " << length
                      << " " << length/2 << endl;

    // Converted Binary to ascii
    char buffer[2];
    for (int i = 0; i < length/2; ++i) {
      input.read(buffer, 2);
      int16_t valueread = static_cast<int16_t>((buffer[1]) | (buffer[0] << 8));
      // int16_t valueread = *(reinterpret_cast<int16_t*>(buffer));

      int32_t bin_32 = valueread;
      std::stringstream sss;
      // converts the integer to hex and fills 0s for empty spaces
      sss << std::hex << std::setfill('0') << std::setw(8) << bin_32;

      string converted_binary = DABnamespace::DecToBitString(valueread, 16);
      input_read_back.push_back(converted_binary);

      //Utils::log_stream << Utils::Format("READ ITEM", 10)
      //<< Utils::Format(i, 3) << ": "
      //<< Utils::Format(valueread, 8) << Utils::Format(sss.str(), 9)
      //<< Utils::Format(converted_binary, 17)
      //<< endl;

      // dumps it into output text file
      out_stream << input_read_back.at(i) << endl;
    }
  }
  input.close();

  //////////////////////////////////////////////////////////////////////////
  // Pass one
  // Produce the symbol table and detect errors in symbols.
  PassOne(in_scanner);
  //PrintCodeLines();
  PrintSymbolTable();
  //////////////////////////////////////////////////////////////////////////
  // Pass two
  // Generate the machine code.
  PassTwo();
  PrintCodeLines();
  //////////////////////////////////////////////////////////////////////////
  // Dump the results.

#ifdef EBUG
  Utils::log_stream << "leave Assemble" << endl;
#endif
}

/***************************************************************************
 * Function 'GetInvalidMessage'.
 * This creates a "value is invalid" error message.
 *
 * Parameters:
 *   leadingtext - the text of what it is that is invalid
 *   symbol - the symbol that is invalid
**/
string Assembler::GetInvalidMessage(string leadingtext, string symbol) {
  string returnvalue = "";
  return returnvalue;
}

/***************************************************************************
 * Function 'GetInvalidMessage'.
 * This creates a "value is invalid" error message.
 *
 * Parameters:
 *   leadingtext - the text of what it is that is invalid
 *   hex - the hex operand that is invalid
**/
string Assembler::GetInvalidMessage(string leadingtext, Hex hex) {
  string returnvalue = "";

  return returnvalue;
}

/***************************************************************************
 * Function 'GetUndefinedMessage'.
 * This creates a "symbol is undefined" error message.
 *
 * Parameters:
 *   badtext - the undefined symbol text
**/
string Assembler::GetUndefinedMessage(string badtext) {
  string returnvalue = "";
  return returnvalue;
}

/***************************************************************************
 * Function 'PassOne'.
 * Produce the symbol table and detect multiply defined symbols.
 *
 * CAVEAT: We have deliberately forced symbols and mnemonics to have
 *         blank spaces at the end and thus to be all the same length.
 *         Symbols are three characters, possibly with one or two blank at end.
 *         Mnemonics are three characters, possibly with one blank at end.
 *
 * Parameters:
 *   in_scanner - the input stream from which to read
 *   out-stream - the output stream to which to write
**/
void Assembler::PassOne(Scanner& in_scanner) {
#ifdef EBUG
  Utils::log_stream << "enter PassOne" << endl;
#endif
  Utils::log_stream << "PASS ONE" << endl;
  // Unfinished code -Sean
  string lbl = "";
  pc_in_assembler_ = 0;
  for (auto it = codelines_.begin(); it != codelines_.end(); it++) {
    if ((*it).IsAllComment()) {
      continue;
    }
    if ((*it).HasLabel()) {
      lbl = (*it).GetLabel();
      if (symboltable_.count(lbl) == 0) {
        symboltable_.insert( {lbl, Symbol(lbl,pc_in_assembler_)} );
      } else {
        symboltable_.at(lbl).SetMultiply();
      }
    }

    if ((*it).GetMnemonic() == "ORG") {
      pc_in_assembler_ = (*it).GetHexObject().GetValue();
    } else if ((*it).GetMnemonic() == "DS ") {
      pc_in_assembler_ += (*it).GetHexObject().GetValue();
    } else {
      pc_in_assembler_++;
    }
    (*it).SetPC(pc_in_assembler_);
    Utils::log_stream << (*it).ToString() << '\n';
 }

#ifdef EBUG
  Utils::log_stream << "leave PassOne" << endl;
#endif
}

/***************************************************************************
 * Function 'PassTwo'.
 * This function does pass two of the assembly process.
**/
void Assembler::PassTwo() {
#ifdef EBUG
  Utils::log_stream << "enter PassTwo" << endl;
#endif
  Utils::log_stream << "PASS TWO" << endl;
  string bitstring = "";
// Note: this code does not handle HEX, ORG, DS, END correctly yet
// This block goes through each of the inputted codelines and converts the
// mnemonic, direct/indirect addressing, and 12 bit address to appropriate
// machine code.
  for (auto it = codelines_.begin(); it != codelines_.end(); it++) {
    // The first block looks for the mnemonic in the map of opcodes, and converts
    // to machine code accordingly.
    if (!(*it).IsAllComment()) {
      map <string, string>::iterator op_it = opcodes.find((*it).GetMnemonic());
      if (op_it != opcodes.end()) {
        bitstring = opcodes.find((*it).GetMnemonic())-> second;
        // TODO do error flagging for if an opcode that isn't in the set of defined codes is found (ex: div)
        // The second block looks for the asterisk to determine whether the 4th bit
        // should be denoted as direct or indirect addressing.
        if((*it).GetAddr() == "*") {
          bitstring += "1";
        }
        else {
          bitstring += "0";
        }
        if (bitstring.substr(0,3) == "111") {
          if ((*it).GetMnemonic() == "RD ") {
            bitstring += "000000000001";
          }
          else if ((*it).GetMnemonic() == "STP") {
            bitstring += "000000000010";
          }
          else if ((*it).GetMnemonic() == "WRT") {
            bitstring += "000000000011";
          }
        }
        else {
        // The third block finds the symbol operand in the symbol table if needed
          map <string, Symbol>::iterator sym_it = symboltable_.find((*it).GetSymOperand());
          Symbol symbol;
          if (sym_it != symboltable_.end()) {
            symbol = symboltable_.find((*it).GetSymOperand())-> second;
            bitstring += DABnamespace::DecToBitString(symbol.GetLocation(), 12);
          }
      // This block handles read, write, and stop, where no value from the symbol
      // table is needed to set the last 12 bits.
          else {
          // TODO The symbol is not found in the symbol table (what do we do here??)
            bitstring += "ERROR0000000";
            Utils::log_stream << "ERROR at " << (*it).GetPC() << endl;
          }
        }
        if(bitstring.length() < 16) {
      // Basically a TEMPORARY hacky way of handling all the errors I haven't handled yet
      // (like stuff like HEX, ORG, END, DS that I just haven't gotten around to generating machine code for
      // and error cases like undefined/multiply defined/invalid symbols and opcodes) 
      // Eventually when all of these cases are handled this block won't be needed and we can remove it 
          (*it).SetMachineCode("0000000000000000");
        }
        else {
        (*it).SetMachineCode(bitstring);
        }
      }
    }
  }
  #ifdef EBUG
    Utils::log_stream << "leave PassTwo" << endl;
  #endif

}

/***************************************************************************
 * Function 'PrintCodeLines'.
 * This function prints the code lines.
**/
void Assembler::PrintCodeLines() {
#ifdef EBUG
  Utils::log_stream << "enter PrintCodeLines" << endl;
#endif
  string s = "";

  for (auto iter = codelines_.begin(); iter != codelines_.end(); ++iter) {
    Utils::log_stream << (*iter).ToString() << '\n';
  }

  if (!found_end_statement_) {
    s += "\n***** ERROR -- NO 'END' STATEMENT\n";
    has_an_error_ = true;
  }

#ifdef EBUG
  Utils::log_stream << "leave PrintCodeLines" << endl;
#endif
  Utils::log_stream << s << endl;
}

/***************************************************************************
 * Function 'PrintMachineCode'.
 * This function prints the machine code.
**/
void Assembler::PrintMachineCode(string binary_filename,
                                 ofstream& out_stream) {
#ifdef EBUG
  Utils::log_stream << "enter PrintMachineCode" << " "
                    << binary_filename << endl;
#endif
  string s = "";

#ifdef EBUG
  Utils::log_stream << "leave PrintMachineCode" << endl;
#endif
}

/******************************************************************************
 * Function 'PrintSymbolTable'.
 * This function prints the symbol table.
**/
void Assembler::PrintSymbolTable() {
#ifdef EBUG
  Utils::log_stream << "enter PrintSymbolTable" << endl;
#endif
  string s = "";
  Utils::log_stream << "SYMBOL TABLE\n    SYM LOC FLAGS\n";
  for (std::pair<string, Symbol> it : symboltable_) {
    Utils::log_stream << "SYM " << it.second.ToString() << endl;
  }

#ifdef EBUG
  Utils::log_stream << "leave PrintSymbolTable" << endl;
#endif
  Utils::log_stream << s << endl;
}

/******************************************************************************
 * Function 'SetNewPC'.
 * This function gets a new value for the program counter.
 *
 * No return value--this sets the class's PC variable.
 *
 * Parameters:
 *   codeline - the line of code from which to update
**/
void Assembler::SetNewPC(CodeLine codeline) {
#ifdef EBUG
  Utils::log_stream << "enter SetNewPC" << endl;
#endif

#ifdef EBUG
  Utils::log_stream << "leave SetNewPC" << endl;
#endif
}

/******************************************************************************
 * Function 'UpdateSymbolTable'.
 * This function updates the symbol table for a putative symbol.
 * Note that there is a hack here, in that the default value is 0
 * and that would mean we can't store a symbol at location zero.
 * So we add one, and then back that out after the full first pass is done.
**/
void Assembler::UpdateSymbolTable(int pc, string symboltext) {
#ifdef EBUG
  Utils::log_stream << "enter UpdateSymbolTable" << endl;
#endif

#ifdef EBUG
  Utils::log_stream << "leave UpdateSymbolTable" << endl;
#endif
}
