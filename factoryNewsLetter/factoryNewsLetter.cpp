// factoryNewsLetter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <ostream>
#include <fstream>
#include <string>
#include <list>

namespace newsstuff
{
	///<summary> abstract base class that represents news items </summary>
	class NewsItem
	{
	public:
		NewsItem( ) {};
		virtual ~NewsItem( ) = 0;
		NewsItem( const NewsItem& ) = default;
		NewsItem& operator=( const NewsItem& ) = default;
		
		// clone requires each type of NewsItem to create
		// a copy of the derivative.
		virtual NewsItem* clone( void ) const = 0;
		virtual std::ostream& out( std::ostream& strm ) const = 0;
	};

	NewsItem::~NewsItem( ) {
		// degenerate virtual destructor
	}

	// derived types of NewsITems
	///<summary>Text represents a type of NewsItem</summary>
	class Text : public NewsItem
	{
	private:
		std::string _text;

	public:
		Text( const std::string& text ) :
			_text( text ) {};
		Text( std::string&& text ) :
			_text( std::move( text ) ) {}
		virtual ~Text( ) {};

		Text( const Text& ) = default;
		Text& operator=( const Text& ) = default;

		const std::string& get( void ) const {
			return _text;
		}

		Text* clone( void ) const override {
			return new Text( *this );
		}

		std::ostream& out( std::ostream& strm ) const override {
			return strm << "Text:" << this->_text;
		}
	};

	///<summary>Picture is also a type of NewsItem</summary>
	class Picture : public NewsItem
	{
	private:
		std::string _pictureLocation;

	public:
		Picture( const std::string& text ) :
			_pictureLocation( text ) {};
		Picture( std::string&& text ) :
			_pictureLocation( std::move( text ) ) {}
		virtual ~Picture( ) {};

		Picture( const Picture& ) = default;
		Picture& operator=( const Picture& ) = default;

		const std::string& get( void ) const {
			return _pictureLocation;
		}

		std::ostream& out( std::ostream& strm ) const override {
			return strm << "Picture:" << this->_pictureLocation;
		}

		Picture* clone( void ) const override {
			return new Picture( *this );
		}
	};

	///<summary>NewsLetter contains multiple news items</summary>
	class NewsLetter
	{
	private:
		std::list<NewsItem*> _items;
		std::string _newsLetterTitle;

		void clearItems( void ) {
			for ( auto& pni : _items ) {
				delete pni;
			}
			_items.clear( );
		}

	public:
		explicit NewsLetter(const std::string& newsLetterTitle ) :
			_newsLetterTitle(newsLetterTitle )
		{}

		NewsLetter(){
		}

		virtual ~NewsLetter(){
			// delete the items
			clearItems( );
		}

		NewsLetter( const NewsLetter& newsletter );
		NewsLetter& operator=( const NewsLetter& newsletter );

		std::ostream& out( std::ostream& stream ) const {
			stream << "Title: " << _newsLetterTitle << "\n";
			for ( auto& pnl : _items ) {
				pnl->out(stream);
				stream << "\n";
			}

			return stream;
		}

		void addNewsItem( const NewsItem& item );
		///<param>filepath contains the fq path to the file to which we save the newsletter</param>
		void save( const std::string& filepath ) const;
		void load( const std::string& filepath );
	};

	/// <summary>
	///  createNewsItem splits the input string to get a type value pair.
	///The type is then used to create the correct deriveative of NewsItem
	/// </summary>
	/// <returns>
	/// NewsItem*.
	/// </returns>
	NewsItem* createNewsItem( const std::string& lineItem )
	{
		std::string type;
		std::string value;

		auto pos = lineItem.find( ":" );
		if ( pos == std::string::npos )
			return nullptr;
		type.assign( lineItem.begin( ), lineItem.begin( ) + pos++ );
		value.assign( lineItem.begin( ) + pos, lineItem.end( ) );

		if ( !type.compare( "Picture" ) ) return new Picture( value );
		if ( !type.compare( "Text" ) ) return new Text( value );
		return nullptr;
	}

	std::ostream& operator<<( std::ostream& stream, const NewsLetter& nl ) {
		return nl.out( stream );
	}
	/// <summary> 
	///	adds a new item to the newsletter
	/// </summary>
	/// <param name="item">Used to provide the newsitem.</param>
	/// <returns> void </returns>
	void NewsLetter::addNewsItem(const NewsItem& item )
	{
		// create a clone and add it to the list
		_items.push_back( item.clone( ) );
	}

	NewsLetter& NewsLetter::operator=( const NewsLetter& nl ) {

		if ( this == &nl ) {
			return *this;
		}

		_newsLetterTitle = nl._newsLetterTitle;
		// delete everything from my list..
		clearItems( );
		

		// create a clone and add it to the list
		for ( auto& ni : nl._items ) {
			this->_items.push_back( ni->clone( ) );
		}

		return *this;
	}
	
	/// <summary> 
	///	NewsLetter copy constructor - deep copy of newsitems
	/// </summary>
	/// <param name="nl">UTHe newsletter we are copying.</param>

	NewsLetter::NewsLetter( const NewsLetter& nl ) : 
		_newsLetterTitle(nl._newsLetterTitle )
	{
		// create a clone and add it to the list
		for ( auto& ni : nl._items ) {
			this->_items.push_back( ni->clone( ) );
		}
	}

	/// <summary> 
	///	NewsLetter copy constructor - deep copy of newsitems
	/// </summary>
	/// <param name="nl">UTHe newsletter we are copying.</param>

	void NewsLetter::save( const std::string& filepath ) const
	{
		std::ofstream fs( filepath );
		std::cout << "Saving newsletter to: " << filepath;
		fs << *this;
		fs.close( );
	}

	std::pair<std::string, std::string> split( const std::string& in,
		const std::string& delimiter )
	{
		auto iterPos = in.find( delimiter );
		if ( std::string::npos != iterPos ) {
			std::string first;
			std::string second;
			std::copy( in.begin( ), in.begin() + (iterPos++), std::back_inserter( first ) );
			std::copy( in.begin( ) + iterPos, in.end( ), std::back_inserter( second ) );

			return std::pair<std::string, std::string>( first, second );
		}

		return std::pair<std::string, std::string>( );

	}
		

	void NewsLetter::load( const std::string& filepath ) 
	{
		std::cout << "Loading newsletter from: " << filepath;
		std::ifstream ifs( filepath );
		if ( !ifs.bad( ) ) {
			// it's a good file
			std::string currentLine;
			bool firstLine = true;
			while( std::getline( ifs, currentLine ) ) {
				if ( firstLine ) {
					this->_newsLetterTitle = split( currentLine, ":" ).second;
					firstLine = false;
				}
				else
					this->_items.push_back( createNewsItem( currentLine ) );
			}

			ifs.close( );
		}
	}

	
} //namespace newsstuff

int main()
{
    std::cout << "Hello World! Creating newsletter...\n";

	{
		using namespace newsstuff;

		NewsLetter weeklyUpdate( "Weekly Update" );
		Text robNote( "This is a note from Rob." );
		Picture robPicture( "RobFlattering.jpg" );

		weeklyUpdate.addNewsItem( robNote );
		weeklyUpdate.addNewsItem( robPicture );

		std::cout << "Newsletter details..\n";
		std::cout << weeklyUpdate << "\n";

		std::cout << "Making a copy of the newsletter...\n";
		NewsLetter copyOfWeeklyUpdate( weeklyUpdate );

		std::cout << "Newsletter details..\n";
		std::cout << copyOfWeeklyUpdate << "\n";

		std::cout << "Adding some items to the copy...\n";
		Text nikNote( "This is a note from Nik" );
		Picture nikPic( "Nik.jpg" );
		copyOfWeeklyUpdate.addNewsItem( nikNote );
		copyOfWeeklyUpdate.addNewsItem( nikPic );

		std::cout << "Copy now looks like: \n" << copyOfWeeklyUpdate << "\n";

		// now assign to the original...
		weeklyUpdate = copyOfWeeklyUpdate;
		std::cout << "After assignment weekly newslatter: \n" << weeklyUpdate << "\n";

		// safe the newsletter
		std::cout << "Saved to " << "c:\\Users\\temp\\weeklyNL.txt\n";
		weeklyUpdate.save( "c:\\Users\\temp\\weeklyNL.txt" );

		// create a new news letter and load it from the saved items..
		NewsLetter restored;
		restored.load( "c:\\Users\\temp\\weeklyNL.txt" );

		std::cout << "Newsletter from loaded saved copy...\n";
		std::cout << restored;
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
