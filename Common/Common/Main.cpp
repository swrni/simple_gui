#include <iostream>

#include <vector>
#include <string>
#include <memory>



class Delegate {
	public:
		Delegate( const std::wstring& str ) : str_( str ) {}
		virtual void Invoke() = 0;
	protected:
		const std::wstring str_;
};

class Delegate1 : public Delegate {
	public:
		using FP1 = void(*)( const std::wstring& );

		Delegate1(	const std::wstring& str, FP1&& fp1 )
				:	Delegate( str ),
					fp1_( fp1 )
		{}
		void Invoke() override { fp1_( str_ ); }
	private:
		FP1 fp1_;
};

class Delegate2 : public Delegate {
	public:
		using FP2 = void(*)( int, const std::wstring& );

		Delegate2(	const std::wstring& str, FP2&& fp2, int number_to_save )
				:	Delegate( str ),
					fp2_( fp2 ),
					saved_number_( number_to_save )
		{}
		void Invoke() override { fp2_( saved_number_, str_ ); }
	private:
		FP2 fp2_;
		int saved_number_;
};

int main() {

	std::vector<Delegate*> dels;
	
	dels.push_back(
		static_cast<Delegate*>(
			new Delegate1(
				L"basic_string",
				[]( const std::wstring& str ) -> void {
					std::wcout << L"fp1, " + str + L"\n";
				}
			)
		)
	);

	dels.push_back(
		static_cast<Delegate*>(
			new Delegate2(
				L"basic_string",
				[]( int number, const std::wstring& str ) -> void {
					std::wcout << L"fp2, " + std::to_wstring( number ) + L", " + str + L"\n";
				},
				13
			)
		)
	);

	for ( auto& del : dels ) {
		del->Invoke();
	}

	system( "pause" );
	return 0;
}