#include "HtmlParser.h"

HtmlParser::HtmlParser() : string_pos(-1), stack_size(1)
{
    HtmlToken root_token = { "root", "" };

    token_stack = new TokenStackNode;
    token_stack->token = root_token;
    token_stack->next = nullptr;
}

HtmlParser::~HtmlParser() {}

void HtmlParser::parseHtmlString (char *string)
{
    // TODO: ensure valid html
    html_string = string;
    string_length = this->getStrLen(string);
    std::cout << string_length << std::endl;
    this->getNextChar();
    this->getToken();
}

void HtmlParser::parseHtmlFromFile (const char *file_name)
{
    std::ifstream infile;

    int file_size = 0;
    char *str = NULL;


    infile.open(file_name);
    if (!infile.is_open())
        throw std::runtime_error("could not open file \n");

    infile.seekg(0, std::ios::end); // go to end of file
    file_size = infile.tellg();     // set file_size to the location (this is the length)
    infile.seekg(0, std::ios::beg); // go back to beginning

    str = new char[file_size+1];
    infile.read(str, file_size);
    str[file_size] = '\0';
    infile.close();

    this->parseHtmlString(str);

    delete [] str;

}

void HtmlParser::getToken ()
{
    this->skipWhitespace();

    if (curChar == '\0') // end of file
    {
        // pop root node 
        while (this->token_stack != nullptr)
            this->tokenStackPop();
        return;
    } 
    else if (curChar == '<') // html tag
    {
        this->getNextChar();
        if (curChar == '/') // closing tag
        {
            this->getNextChar();

            std::string closingTag = this->readUntil('>');
            this->tokenStackPop();

            this->getNextChar();
            this->getToken();
        } 
        else if (curChar == '!') // could be doctype or comment 
        {
            this->getNextChar();
            this->skipWhitespace();
            if (curChar == '-' && this->peekNext() == '-') // comment
                this->ignoreComment();
            else if (this->checkValidDoctype()) // check for doctype
                this->parseDoctype();
            else
                throw std::runtime_error("error parsing exclamation opening tag");
            this->getToken();
            
        }
        else // opening tag
        {
            HtmlToken nextToken;
            nextToken.token_type = this->readUntil('>');
            this->tokenStackPush(nextToken);

            this->getNextChar();
            this->getToken();
        }
    } 
    else // content between tags
    {
        // parse content and add to top of token_stack's content string
        std::string content = this->readUntil('<');
        token_stack->token.token_content = content;
        this->getToken();
    }
}

void HtmlParser::parseDoctype ()
{
    while (curChar != ' ')
        this->getNextChar();
    this->skipWhitespace();
    doctype = this->readUntil('>');
    this->getNextChar();
}

bool HtmlParser::checkValidDoctype () const
{
    const char *doctypeStr = "DOCTYPE";
    int pos = 0;
    while (pos < 7)
    {
        if (toupper(html_string[string_pos + pos]) != toupper(doctypeStr[pos]))
            return false;
        pos++;
    }
    return true;
}

void HtmlParser::ignoreComment ()
{
    bool commentEnd = false;
    while (!commentEnd)
    {
        while (curChar != '>')
        {
            this->getNextChar();
        }
        if (html_string[string_pos-1] == '-' && html_string[string_pos-2] == '-')
            commentEnd = true;
        this->getNextChar();
    }
}

std::string HtmlParser::readUntil (char until)
{
    std::string ret;
    while (curChar != until) {
        ret.append(1, curChar);
        this->getNextChar();
    }
    return ret;
}

void HtmlParser::getNextChar ()
{
    string_pos++; 
    if (string_pos <= string_length)
        curChar = html_string[string_pos];
}

void HtmlParser::skipWhitespace ()
{
    while (isspace(curChar)) {
        this->getNextChar();
    }
}

char HtmlParser::peekNext() const
{
    if (string_pos > string_length)
        throw std::runtime_error("peeked off end of html string\n");
    return html_string[string_pos+1];
}

void HtmlParser::tokenStackPush(struct HtmlToken token)
{
    struct TokenStackNode *new_node = new TokenStackNode;
    new_node->token = token;
    new_node->next = token_stack;
    token_stack = new_node;
    stack_size++;

    new_node = nullptr;
}

void HtmlParser::tokenStackPop()
{
    struct TokenStackNode *temp = token_stack->next;

    if (!token_stack->token.token_content.empty())
    {
        std::cout << token_stack->token.token_content << std::endl;
    }

    delete token_stack;
    token_stack = temp;
    stack_size--;

    temp = nullptr;
}

int HtmlParser::getStrLen(char *string) const
{
    int len = 0;
    while (string[len] != '\0')
        len++;
    return len;
}
