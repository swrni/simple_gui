#pragma once

#include <memory>
#include <vector>

#include "Common.hpp"
#include "sfml_includes.hpp"

#include "Text.hpp"


namespace GUI {

enum class MouseEventType {
	kIdle,
	kOnDrag,
	kOnSelect,
	kOnUnselect,
	kOnHover,
};
using MouseEventHandler = EventHandler<MouseEventType>;

struct ButtonTheme {
	const sf::Vector2f size_ = { 140.0f, 24.0f };

	const sf::Color outline_color_ = sf::Color::Black;
	const sf::Color fill_color_ = sf::Color::Transparent;

	const float outline_thickness_ = 1.0f;

	const TextTheme text_theme_ = {};
};

struct ButtonThemes {
	using EventsToButtons = const std::map<MouseEventType, ButtonTheme>;
	const EventsToButtons events_to_buttons_ = {
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
	const EventsToButtons& operator()() const {
		return events_to_buttons_;
	}
};

class Button : NoCopy {
	public:
		explicit Button(	sf::RenderWindow& render_window,
							sf::Vector2f position,
							const ButtonThemes& button_themes = {} )
						:	render_window_( render_window ),
							position_( position ),
							button_themes_( button_themes ),
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
				return button_themes_().at( current_event_type_ );
			}
			catch ( std::out_of_range ) {
				return button_themes_().at( MouseEventType::kIdle );
			}
		}

		inline void CreateEventHandler(
			const MouseEventType type,
			const MouseEventHandler::Callback callback,
			const void* saved_arg = nullptr )
		{
			events_.push_back(
				{ type, callback, saved_arg }
			);
		}

		inline void OnEvent(	const MouseEventType& event_type,
								bool force_change = false )
		{
			if ( !force_change ) {
				if ( !IsStateChangeAllowed( event_type ) ) {
					return;
				}
			}

			setCurrentTheme( event_type );
			for ( auto& event : events_ ) {
				if ( event.type_ == event_type ) {
					event.Invoke();
				}
			}
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
			const bool dragging =
				( current_event_type_ == MouseEventType::kOnDrag );

			// New events.
			const bool start_hoovering =
				( new_event_type == MouseEventType::kOnHover );

			if ( dragging && start_hoovering )
				return false;

			return true;
		}

		sf::RenderWindow& render_window_;
		const sf::Vector2f position_;
		const ButtonThemes button_themes_;
		TextWrapper text_wrapper_;

		MouseEventType current_event_type_ = MouseEventType::kIdle;
		std::vector<MouseEventHandler> events_ = {};
};


};