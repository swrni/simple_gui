#pragma once

#include "sfml_includes.hpp"

#include "Button.hpp"


namespace GUI {


struct TextEdit {
	public:
		using TextEditThemes = ButtonThemes;
		
		static const TextEditThemes& DefaultThemes() {
			static const TextEditThemes themes = { {
				{ MouseEventType::kIdle, {
						{ 180.0f, 24.0f },
					}
				},
				{ MouseEventType::kOnSelect, {
						{ 180.0f, 24.0f },
						sf::Color( 120, 120, 120 )
					}
				}
			} };
			return themes;
		}

		TextEdit(	sf::RenderWindow& render_window,
					const sf::Vector2f& position,
					const TextEditThemes& text_edit_themes = DefaultThemes() )
				:	button_( render_window, position, text_edit_themes ),
					editing_( false )
		{}
		
		inline void Render() const {
			button_.Render();
		}

		inline void OnEvent( const MouseEventType& event_type ) {
			if ( !IsStateChangeAllowed( event_type ) )
				return;

			button_.OnEvent( event_type, true );
		}

		inline void OnKeyboardInput( const sf::Event& event ) {
			if ( !editing_ ) return;
			if ( !can_edit_ ) return;

			if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Return) ) {
				OnEvent( MouseEventType::kOnUnselect );
				return;
			}
	
			// Wait timer.
			button_.getText().ReadInput(
				event,
				button_.CurrentTheme().size_
			);
		}

		inline sf::FloatRect getBounds() const {
			return button_.getBounds();
		}

	private:
		inline bool IsStateChangeAllowed( const MouseEventType& new_event_type ) {
			if ( new_event_type == MouseEventType::kOnSelect ) {
				editing_ = true;
			}
			else if ( new_event_type == MouseEventType::kOnUnselect ) {
				editing_ = false;
			}
			else {
				return false;
			}
			return true;
		}

		Button button_;
		bool editing_;
		const bool can_edit_ = true;
};


};