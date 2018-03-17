#pragma once

#include <vector>
#include <string>
#include <memory>

struct NoCopy {
	NoCopy() = default;
	NoCopy( const NoCopy& ) = delete;
	const NoCopy& operator=( const NoCopy& ) = delete;
};

template <typename _Arg>
class Function {
	public:
		using Arg = _Arg;
		using Callback = void(*)( const void*, const Arg& );
	public:
		Function(	Callback callback = nullptr,
					const void* saved_args = nullptr )
				:	callback_( callback ),
					saved_args_( saved_args )
		{}

		Callback callback_;
		const void* saved_args_;

		void Invoke( const Arg& data ) const {
			return callback_( saved_args_, data );
		}
};

template <typename T1, typename T2>
class Union {
	public:
		Union() : state_( State::kUnset ) {}
		Union(	const T1* t1 )
			:	state_( State::kT1 )
		{
			t1_ = t1;
		}
		Union(	const T2* t2 )
			:	state_( State::kT2 )
		{
			t2_ = t2;
		}

		template <typename ErrorType>
		constexpr ErrorType get() const { throw; }

		template <>
		constexpr const T1* get<const T1*>() const {
			// TODO:: Fix constexprness and ctor.
			// if constexpr ( state_ != State::kT1 ) throw;
			if ( state_ != State::kT1 ) throw;
			return t1_;
		}

		template <>
		constexpr const T2* get<const T2*>() const {
			// TODO:: Fix constexprness and ctor.
			// if constexpr ( state_ != State::kT2 ) throw;
			if ( state_ != State::kT2 ) throw;
			return t2_;
		}

		template <typename T>
		void set( const T* ) { throw; }

		constexpr void set( const T1* t1 ) {
			t1_ = t1;
			state_ = kT1;
		}
		constexpr void set( const T2* t2 ) {
			t2_ = t2;
			state_ = kT2;
		}
		bool operator==( const Union& other ) const {
			if ( state_ != other.state_ ) {
				return false;
			}
			if ( state_ == State::kT1 ) {
				return ( t1_ == other.t1_ );
			}
			else {
				return ( t2_ == other.t2_ );
			}
		}

	private:
		union {
			const T1* t1_;
			const T2* t2_;
		};

		enum class State {
			kUnset,
			kT1,
			kT2,
		} state_;
};

struct DataWrapperBase {};

template <typename StoredObject>
struct DataWrapper final : public DataWrapperBase {
	using Derived = DataWrapper<StoredObject>;
	using DerivedP = const Derived*;
	
	static DerivedP Cast( const DataWrapperBase* base ) {
		return static_cast<DerivedP>( base );
	}

	DataWrapper(	StoredObject&& stored_object )
				:	stored_object_( std::move( stored_object ) )
	{}
	StoredObject stored_object_;
};

class EventHandler {
	public:
		using Callback = void(*)( const DataWrapperBase* );
		EventHandler(	Callback callback,
						std::unique_ptr<DataWrapperBase> stored_data )
					:	callback_( callback ),
						stored_data_( std::move( stored_data ) )
		{}
		void Invoke() const {
			callback_( stored_data_.get() );
		}
		void Invoke() {
			callback_( stored_data_.get() );
		}
	private:
		const Callback callback_;
		std::unique_ptr<DataWrapperBase> stored_data_;
};


inline std::string ToString( const std::wstring& w_str ) {
	return std::string( std::begin( w_str ), std::end( w_str ) );
}

inline void TrimString( std::wstring& str, const std::wstring& cs_to_save );

void CleanPath( std::wstring& path );

bool FindPath( std::wstring& path );
bool FindAndCleanPath( std::wstring& path );

constexpr inline bool XOR( const bool b1, const bool b2 ) {
	return ( b1 && !b2 ) || ( !b1 && b2 );
}

void ToLower( std::wstring& str );

void PrintItems( const std::vector<std::wstring>& items );

