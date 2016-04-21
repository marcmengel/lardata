/**
 * @file   NewLine.h
 * @brief  Simple class managing a repetitive output task
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   December 18th, 2015
 */

#ifndef LARDATA_RECOBASE_DUMPERS_NEWLINE_H
#define LARDATA_RECOBASE_DUMPERS_NEWLINE_H 1

// C/C++ standard libraries
#include <string>


namespace recob {
  namespace dumper {
    
    /**
     * @brief Starts a new line in a output stream
     * @tparam Stream type of output stream
     * 
     * Example of usage:
     *     
     *     std::cout << "Preamble on its own line." << std::endl;
     *     NewLine OutLn(std::cout, "> ");
     *     OutLn() << "An indented line.";
     *     OutLn() << "Another indented line.";
     *     
     * that (after flush) will result in the output
     *     
     *     Preamble on its own line.
     *     
     *     > An indented line.
     *     > Another indented line.
     *     
     * Asking to consider the first line already started, instead:
     *     
     *     std::cout << "Preamble on its own line." << std::endl;
     *     NewLine OutLn(std::cout, "> ", true);
     *     OutLn() << "An indented line.";
     *     OutLn() << "Another indented line.";
     *     
     * will instead result in the output
     *     
     *     Preamble on its own line.
     *     > An indented line.
     *     > Another indented line.
     *     
     * (note that the line that we consider started was actually an empty one).
     */
    template <typename Stream>
    class NewLine {
        public:
      /**
       * @brief Constructor: associates with the stream
       * @param stream a reference to the stream where to insert new lines
       * @param indent string used for indentation (default: none)
       * @param followLine whether first line is already started (default: no)
       * 
       * The constructor does not start a new line.
       * If followLine is true, the first line is supposed to be already started
       * and no indentation nor new line will be set on it.
       */
      NewLine(Stream& stream, std::string indent = "", bool followLine = false)
        : out(stream), indentString(indent), nLines(0), appendFirst(followLine)
        {}
      
      /// @{
      /// @name Accessors
      
      /// Returns the number of inserted lines
      unsigned int lines() const { return nLines; }
      
      /// Returns the current indentation string
      std::string indent() const { return indentString; }
      
      /// @}
      
      /// Starts a new line
      Stream& newLine() { if (!append()) forceNewLine(); ++nLines; return out; }
      
      /// Calls and returns newLine(). Candy.
      Stream& operator() () { return newLine(); }
      
      /// Starts a new line (no matter what)
      void forceNewLine() { out << "\n" << indentString; }
      
      /// Returns whether newLine() will append text on the current line
      bool append() const { return (lines() == 0) && appendFirst; }
      
      
      /// Replaces the indentation string
      void setIndent(std::string newIndent) { indentString = newIndent; }
      
      /// Adds to the end to the indentation string
      void addIndent(std::string moreIndent) { indentString += moreIndent; }
      
      
        protected:
      Stream& out; ///< reference to the output stream
      std::string indentString; ///< full indentation string
      unsigned int nLines; ///< number of lines in output
      bool appendFirst; ///< whether to append to the first line
      
    }; // class NewLine
    
    
    /// Convenience function to create a temporary NewLine
    template <typename Stream>
    inline NewLine<Stream> makeNewLine
      (Stream& stream, std::string indent = "", bool followLine = false)
      { return NewLine<Stream>(stream, indent, followLine); }
    
    
  } // namespace dumper
} // namespace recob


#endif // LARDATA_RECOBASE_DUMPERS_NEWLINE_H
