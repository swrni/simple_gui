#pragma once

#include <memory>
#include <vector>

#include "Common.hpp"
#include "sfml_includes.hpp"

#include "Text.hpp"


namespace GUI {

enum class EventType {
	kMouseEvent,
	kKeyboardEvent,
};
enum class MouseEventType {
	kIdle,
	kOnDrag,
	kOnSelect,
	kOnUnSelect,
	kOnHover,
};
// using MouseEventHandler = EventHandler<MouseEventType>;
using MouseEventHandler = EventHandler;

struct ButtonTheme {
	const sf::Vector2f size_ = { 140.0f, 24.0f };

	const sf::Color outline_color_ = sf::Color::Black;
	const sf::Color fill_color_ = sf::Color::Transparent;

	const float outline_thickness_ = 1.0f;

	const TextTheme text_theme_ = {};
};

struct ButtonThemes {
	using EventsToThemes = const std::map<MouseEventType, ButtonTheme>;
	const EventsToThemes events_to_themes_ = {
		{ MouseEventType::kIdle, {} },
		{ MouseEventType::kOnSelect, {
				{ 140.0f, 24.0f },
				sf::Color( 100, 100, 100 ),
				sf::Color::Transparent
			}
		},
		{ MouseEventType::kOnHover, {
				{ 140.0f, 24.0f },
				sf::Color( 100, 100, 100 ),
				sf::Color::Transparent
			}
		},
		{ MouseEventType::kOnDrag, {
				{ 140.0f, 24.0f },
				sf::Color( 100, 100, 100 ),
				sf::Color( 0, 0, 0, 10 )
			}
		}
	};
};

class Button : NoCopy {
	public:
		// TODO:: Remove me!
		/*explicit Button(	sf::RenderWindow& render_window,
							sf::Vector2f position,
							const ButtonThemes& button_themes = {} )
						:	render_window_( render_window ),
							position_( position ),
							button_themes_( button_themes ),
							text_wrapper_( position_ )
		{
			text_wrapper_.Initialize( CurrentTheme().text_theme_ );
		}*/
		explicit Button(	sf::RenderWindow& render_window,
							sf::Vector2f position,
							const ButtonThemes::EventsToThemes& events_to_themes = {} )
						:	render_window_( render_window ),
							position_( position ),
							events_to_themes_( events_to_themes ),
							text_wrapper_( position_ )
		{
			text_wrapper_.Initialize( CurrentTheme().text_theme_ );
		}

		inline void Render() const {
			const ButtonTheme& current_theme = CurrentTheme();

			sf::RectangleShape shape( current_theme.size_ );

			shape.setFillColor( current_theme.fill_color_ );
			shape.setOutlineColor( current_theme.outline_color_ );
			shape.setOutlineThickness( current_theme.outline_thickness_ + 0.0001f );
		
			shape.setPosition( position_ );

			render_window_.draw( shape );
			render_window_.draw( text_wrapper_.getText() );
		}

		inline sf::FloatRect getBounds() const {
			return {
				position_,
				CurrentTheme().size_
			};
		}

		inline void setText( const sf::String& string ) {
			text_wrapper_.setText(
				string,
				CurrentTheme().size_
			);
		}
		
		inline const std::wstring ToString() const {
			return text_wrapper_.getText().getString().toWideString();
		}

		inline const ButtonTheme& CurrentTheme() const {
			try {
				return events_to_themes_.at( current_event_type_ );
			}
			catch ( std::out_of_range ) {
				std::cout << "[Button::CurrentTheme] std::out_of_range. current_event_type_: " +
					std::to_string( (int)current_event_type_ ) + "\n";
				return events_to_themes_.at( MouseEventType::kIdle );
			}
		}
		
		/*inline void CreateEventHandler(	const MouseEventType type,
										const MouseEventHandler::Callback callback,
										const void* saved_arg = nullptr )
		{
			events_.push_back(
				{ type, callback, saved_arg }
			);
		}*/

		inline void CreateEventHandler(	const MouseEventType event_type,
										const MouseEventHandler::Callback callback,
										std::unique_ptr<DataWrapperBase> stored_data )
		{
			/*int item = 3;
			std::unique_ptr<DataWrapperBase> stored_data = std::make_unique<DataWrapper<int>>(
				std::move( item )
			);*/
			MouseEventHandler handler(
				callback,
				std::move( stored_data )
			);
			// events_.push_back( std::move( handler ) );
			events_[ event_type ].push_back(
				std::move( handler )
			);
		}
		template <typename T>
		void CreateEventHandler(	const MouseEventType event_type,
									const MouseEventHandler::Callback callback,
									T object )
		{
			std::unique_ptr<DataWrapperBase> stored_data =
				std::make_unique<DataWrapper<T>>( std::move( object ) );

			CreateEventHandler(
				event_type,
				callback,
				std::move( stored_data )
			);
		}
		
		/*inline void OnEvent(	const MouseEventType& event_type,
								bool force_change = false )
		{
			if ( !force_change ) {
				if ( !IsStateChangeAllowed( event_type ) ) {
					return;
				}
			}

			setCurrentTheme( event_type );
			for ( auto& event : events_[ event_type ] ) {
				event.Invoke();
			}
			
			if ( current_event_type_ == MouseEventType::kOnSelect ) {
				// std::cout << "[Button::OnEvent] kOnSelect\n";
			}
			if ( current_event_type_ == MouseEventType::kOnUnSelect ) {
				std::cout << "[Button::OnEvent] kOnUnSelect\n";
				OnEvent( MouseEventType::kIdle, true );
			}
		}*/
		
		inline void OnEventForceChange( const MouseEventType& event_type ) {
			setCurrentTheme( event_type );
			for ( auto& event : events_[ event_type ] ) {
				event.Invoke();
			}
			if ( current_event_type_ == MouseEventType::kOnUnSelect ) {
				// std::cout << "[Button::OnEvent] kOnUnSelect\n";
				OnEventForceChange( MouseEventType::kIdle );
			}
		}
		inline void OnEvent( const MouseEventType& event_type ) {
			if ( IsStateChangeAllowed( event_type ) ) {
				OnEventForceChange( event_type );
			}
		}
		
		inline void OnKeyboardInput( const sf::Event& event ) {
			std::cout << "[Button::OnKeyboardInput]\n";
		}

		constexpr inline TextWrapper& getText() {
			return text_wrapper_;
		}

	private:
		inline void setCurrentTheme( const MouseEventType& type ) {
			current_event_type_ = type;
		}

		inline bool IsStateChangeAllowed( const MouseEventType& new_event_type ) {
			// Current events.
			const bool dragging = ( current_event_type_ == MouseEventType::kOnDrag );

			// New events.
			const bool start_hoovering =
				( new_event_type == MouseEventType::kOnHover );

			if ( dragging && start_hoovering )
				return false;

			return true;
		}

		sf::RenderWindow& render_window_;
		const sf::Vector2f position_;
		const ButtonThemes::EventsToThemes events_to_themes_ = {
			{ MouseEventType::kIdle, {} },
			{ MouseEventType::kOnSelect, {
					{ 140.0f, 24.0f },
					sf::Color( 100, 100, 100 ),
					sf::Color::Transparent
				}
			},
			{ MouseEventType::kOnHover, {
					{ 140.0f, 24.0f },
					sf::Color( 100, 100, 100 ),
					sf::Color::Transparent
				}
			},
			{ MouseEventType::kOnDrag, {
					{ 140.0f, 24.0f },
					sf::Color( 100, 100, 100 ),
					sf::Color( 0, 0, 0, 10 )
				}
			}
		};
		TextWrapper text_wrapper_;

		MouseEventType current_event_type_ = MouseEventType::kIdle;
		// std::vector<MouseEventHandler> events_ = {};
		std::map<MouseEventType, std::vector<MouseEventHandler>> events_;
};


};