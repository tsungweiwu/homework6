#include "pullet16assembler.h"

/***************************************************************************
 *3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456
 * Class 'Assembler' for assembling code.
 *
 * Author/copyright:  Duncan A. Buell.  All rights reserved.
 * Used with permission and modified by: Group 5:
   Tsung Wei Wu, Sean Wiig, Samyu Comandur, Mark Mcmurtury, Mayank Patel.
 * Date: 4 December 2018
**/

/***************************************************************************
 * Constructor
**/
Assembler::Assembler() {
}
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

  //////////////////////////////////////////////////////////////////////////
  // Pass one
  // Produce the symbol table and detect errors in symbols.
  PassOne(in_scanner);
  PrintCodeLines();
  PrintSymbolTable();
  //////////////////////////////////////////////////////////////////////////
  // Pass two
  // Generate the machine code.
  PassTwo();
  PrintCodeLines();
  PrintSymbolTable();
  if (!has_an_error_ == true) {
    PrintMachineCode(binary_filename, out_stream);
  } else {
    Utils::log_stream << "ERRORS EXIST:\nNO MACHINE CODE GENERATED:" << endl;
  }
  //////////////////////////////////////////////////////////////////////////
  // Dump the results.
  // Reading Binary File
  std::ifstream input(binary_filename, std::ifstream::binary);

  if (input) {
  input.seekg(0, input.end);
  int length = input.tellg();
  input.seekg(0, input.beg);
  char* buffer = new char[1];
  for (int i = 0; i < length/2 ; ++i) {
    input.read(buffer, 1);           // read one character
    unsigned char c1 = *buffer;      // dereference
    input.read(buffer, 1);           // read one character
    unsigned char c2 = *buffer;      // dereference
    // Convert the characters back to a decimal
    int bin_to_dec = c1 * 256;
    int16_t bin_16 = bin_to_dec + c2;
    string bin_to_bitstring = DABnamespace::DecToBitString(bin_16, 16);
    // dumps to adotout text file
    out_stream << bin_to_bitstring << endl;
  }  // end of for loop
input.close();
}

#ifdef EBUG
  Utils::log_stream << "leave Assemble" << endl;
#endif
}

/***************************************************************************
 * Function 'GetInvalidMessage'.
 * This creates a "value is invalid" error message.
 *
 * Parameters:
 *   errortext - whatever it is that is invalid
**/
string Assembler::GetInvalidMessage(string errortext) {
  string returnvalue = "";
  has_an_error_ = true;
  returnvalue = "\n***** ERROR -- " + errortext + " IS INVALID";
  return returnvalue;
}

/***************************************************************************
 * Function 'GetUndefinedMessage'.
 * This creates a "symbol is undefined" error message.
 *
 * Parameters:
 *   errortext - the undefined symbol text
**/
string Assembler::GetUndefinedMessage(string errortext) {
  string returnvalue = "";
  has_an_error_ = true;
  returnvalue = "\n***** ERROR -- SYMBOL " + errortext + " IS UNDEFINED";
  return returnvalue;
}

/***************************************************************************
 * Function 'GetMultiplyMessage'.
 * This creates a "value is multiply defined" error message.
 *
 * Parameters:
 *   errortext - the symbol that is invalid
**/
string Assembler::GetMultiplyMessage(string errortext) {
  string returnvalue = "";
  has_an_error_ = true;
  returnvalue = "\n***** ERROR -- " + errortext + " IS MULTIPLY DEFINED";
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
  found_end_statement_ = 0;
  string lbl = "";
  pc_in_assembler_ = 0;
  maxpc_ = 4096;
  int linecounter_ = 0;
  has_an_error_ = false;
  CodeLine codeline;
  // obtaining the next line read
  string line = in_scanner.NextLine();
  while (line != "" && pc_in_assembler_ < maxpc_) {
    codeline = CodeLine();  // creating object
    string mnemonic, label, addr, symoperand, hexoperand, comments, code;

    if (line.length() < 21) {
      line.append(21-line.length(), ' ');  // pad to length 21, for easier code
    }  // end of if

    if (line.substr(0, 1) == "*") {
      // set beginning lines of comments
      codeline.SetCommentsOnly(linecounter_, line);
      // set machine code to null for the comments
      code = "nullcode";
      codeline.SetMachineCode(code);
    } else {
      if (line.substr(0, 3) != "   ") {  // reading label
        label = line.substr(0, 3);
        if (symboltable_.count(label) == 0) {  // checks for duplicate
          // add symbol to table if not already present
          symboltable_.insert({label, Symbol(label, pc_in_assembler_)});
        } else {  // else found duplicate symbol
          symboltable_.at(label).SetMultiply();  // sets multiply flag
        }  // end else
      }  // end of if label
      mnemonic = line.substr(4, 3);  // getting mnemonic
      addr = line.substr(8, 1);  // getting addr
      if (line.substr(10, 3) != "   ") {  // getting symbol
        // if field is empty, value of symoperand_ will be null
        symoperand = line.substr(10, 3);
      }  // end of if symbol
      hexoperand = line.substr(14, 5);  // getting hex value
      if (line.substr(20, 1) == "*") {  // checks if it is a comment
        comments = line.substr(20);  // getting comment
      }  // end of if comments
      // sends all the data for it to be assigned
      codeline.SetCodeLine(linecounter_, pc_in_assembler_, label,
      mnemonic, addr, symoperand, hexoperand, comments, kDummyCodeA);
      int jumpvalue = codeline.GetHexObject().GetValue();
      if (mnemonic == "ORG") {
        if (!(jumpvalue > 4095 || jumpvalue < 0)) {
          pc_in_assembler_ = jumpvalue;  // ORG to hex value
        }  // end of if ORG
      } else if (mnemonic == "DS ") {
        if (!((jumpvalue + pc_in_assembler_) > 4095 || jumpvalue < 1)) {
        pc_in_assembler_ += jumpvalue;  // DS to hex value
        }  // end of if DS
      } else if (mnemonic != "END") {
        pc_in_assembler_++;  // PC increases after every instr
      }  // end of if END

      if (mnemonic == "END") {
        found_end_statement_ = true;  // found end which sets has_error false
      }  // end of if statement END
    }  // end of else statement
    linecounter_++;
    codelines_.push_back(codeline);
    line = in_scanner.NextLine();  // will get next line until file is empty
  }  // end of while loop

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
  string err;
  Symbol symbol;
  Hex hex;
  // This block goes through each of the inputted codelines and converts the
  // mnemonic, direct/indirect addressing, and 12 bit address to appropriate
  // machine code.
  for (auto it = codelines_.begin(); it != codelines_.end(); it++) {
    // If the line is a comment, then ignore
    if ((*it).IsAllComment()) {
      continue;
    }
    hex = (*it).GetHexObject();
    err = "";
    // The first block looks for the mnemonic in the map
    // of opcodes, and converts to machine code accordingly.
    map <string, string>::iterator op_it = opcodes.find((*it).GetMnemonic());
    if (op_it != opcodes.end()) {
      bitstring = opcodes.find((*it).GetMnemonic())-> second;
      // check for the asterisk to determine the addressing bit
      if ((*it).GetAddr() == "*") {
        bitstring += "1";
      } else {
        bitstring += "0";
      }  // end of if indirect addr

      // The last twelve digits are defined by the symbol or the hex operand,
      // unless it's RD or STP or WRT
      map <string, Symbol>::iterator sym_it = symboltable_.find(
                                                (*it).GetSymOperand());
      // If symbol is in symboltable use it to complete bitstring
      if (sym_it != symboltable_.end()) {
        symbol = sym_it -> second;
        bitstring += DABnamespace::DecToBitString(symbol.GetLocation(), 12);
      } else if ((*it).HasSymOperand()) {
        // If there was no match in the symboltable, check if the codeline has
        // a symbol. If it does then it's an undefined symbol.
        bitstring = kDummyCodeC;
      } else if (!hex.IsNull()) {  // If no symbol, check for a hex operand.
        if (hex.GetValue() < 0 || hex.GetValue() > 4095) {
          err += GetInvalidMessage("HEX OPERAND " + hex.GetText());
        } else {
          bitstring += DABnamespace::DecToBitString(hex.GetValue(), 12);
        }  // end of else
      } else {  // If there was no hex or symbol
        err += "\n***** ERROR -- NO HEX OR SYMBOL OPERAND";
        bitstring = kDummyCodeC;
      }  // end of else
      WriteMemory((*it).GetPC(), bitstring);  // for PrintMachineCode
      (*it).SetMachineCode(bitstring);  // for PrintCodeLine
    } else if ((*it).GetMnemonic() == "RD ") {
      // check the other possible opcodes.
      // If the mnemonic wasn't in the map of opcodes,
      bitstring = "1110000000000001";
      WriteMemory((*it).GetPC(), bitstring);
      (*it).SetMachineCode(bitstring);  // for PrintCodeLine
    } else if ((*it).GetMnemonic() == "STP") {
      bitstring = "1110000000000010";
      WriteMemory((*it).GetPC(), bitstring);
      (*it).SetMachineCode(bitstring);  // for PrintCodeLine
    } else if ((*it).GetMnemonic() == "WRT") {
      bitstring = "1110000000000011";
      WriteMemory((*it).GetPC(), bitstring);
      (*it).SetMachineCode(bitstring);  // for PrintCodeLine
    } else if ((*it).GetMnemonic() == "HEX") {
      bitstring = DABnamespace::DecToBitString(
          (*it).GetHexObject().GetValue(), 16);
      WriteMemory((*it).GetPC(), bitstring);
      (*it).SetMachineCode(bitstring);  // for PrintCodeLine
    } else if ((*it).GetMnemonic() == "DS ") {
      if ((hex.GetValue() + pc_in_assembler_) > 4095 || hex.GetValue() < 1) {
        err += GetInvalidMessage("DS ALLOCATION "+ hex.GetText());
      } else {
        WriteMemory((*it).GetPC() +
            (*it).GetHexObject().GetValue() - 1, kDummyCodeA);
        WriteMemory((*it).GetPC(), kDummyCodeC);
      }  // end of if DS
    } else if ((*it).GetMnemonic() == "ORG") {
      if (hex.GetValue() > 4095 || hex.GetValue() < 0) {
        err += GetInvalidMessage("ORG ALLOCATION " + hex.GetText());
      }  // end of if ORG
    } else if ((*it).GetMnemonic() == "END") {
      bitstring = kDummyCodeD;
      (*it).SetMachineCode(bitstring);  // for PrintCodeLine
    } else {
      // If everything failed to match, then the mnemonic is not valid.
      err += GetInvalidMessage("MNEMONIC '" + (*it).GetMnemonic() + "\'");
    }  // end of else

    // Check hex value's error status
    if (hex.HasAnError()) {
      err += hex.GetErrorMessages();
    }

    // Check Label for its error flags
    if ((*it).HasLabel()) {
      Symbol label = symboltable_.find((*it).GetLabel()) -> second;
      if (label.IsInvalid()) {  // checks for invalid labels
        err += GetInvalidMessage("LABEL '" + (*it).GetLabel() + "\'");
      }
      if (label.IsMultiply()) {  // checks for duplication
        err += GetMultiplyMessage("LABEL '" + (*it).GetLabel() + "\'");
      }
    }  // end of if HasLabel

    // Finally, check the symoperand for its error flags
    if ((*it).HasSymOperand()) {
      auto sym_it = symboltable_.find((*it).GetSymOperand());
      if (sym_it == symboltable_.end()) {  // loops through table
        err += GetUndefinedMessage("\'" + (*it).GetSymOperand() + "\'");
      } else {
        symbol = sym_it -> second;
        if (symbol.IsInvalid()) {  // checks for invalid symbols
          err += GetInvalidMessage("SYMBOL '" + (*it).GetSymOperand() + "\'");
        }
        if (symbol.IsMultiply()) {  // checks for duplication
          err += GetMultiplyMessage("SYMBOL '" + (*it).GetSymOperand() + "\'");
        }
      }
    }  // end of if HasSymOperand

    (*it).SetErrorMessages(err);
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
// Dumps Machine Code stored in OneMemoryCode in a pretty format
Utils::log_stream << "MACHINE CODE\n"
                  << "enter PrintMachineCode " << binary_filename
                  << endl;

std::ofstream output(binary_filename, std::ofstream::binary);
int count = 0;
for (auto iter = memory_.begin(); iter != memory_.end(); ++iter) {
  Utils::log_stream << Utils::Format(count, 4) << " "
  << DABnamespace::DecToBitString(count, 12) << " "
  << (*iter).GetBitPattern().substr(0, 4) << " "
  << (*iter).GetBitPattern().substr(4, 4) << " "
  << (*iter).GetBitPattern().substr(8, 4) << " "
  << (*iter).GetBitPattern().substr(12, 4) << "\n";
  count++;

  if (output) {
    int16_t ascii_16 = DABnamespace::BitStringToDec((*iter).GetBitPattern());
    char data[4];
      data[0] = static_cast<char>(ascii_16 >> 8);
      data[1] = static_cast<char>((ascii_16));
      // writes the binary to file
      output.write(data, 2);
  }  // end of if (output)
}  // end of for loop
output.close();
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

Utils::log_stream << "SYMBOL TABLE\n    SYM LOC FLAGS\n";
for (std::pair<string, Symbol> it : symboltable_) {
  Utils::log_stream << "SYM " << it.second.ToString() << endl;
}
  Utils::log_stream << endl;
#ifdef EBUG
  Utils::log_stream << "leave PrintSymbolTable" << endl;
#endif
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

void Assembler::WriteMemory(int pc, string code) {
  for (int i = memory_.size(); i < pc; i++) {
    memory_.push_back(OneMemoryWord(kDummyCodeA));
  }
  if (static_cast<int>(memory_.size()) == pc) {
    memory_.push_back(OneMemoryWord(code));
  } else {
    memory_.at(pc) = OneMemoryWord(code);
  }
}
