#pragma once

#include <memory>
#include <vector>

#include "sfml_includes.hpp"

#include "TextEdit.hpp"

#pragma warning( disable : 4706 ) // For conditional assignment: "if ( int* i = getI() ) {}"


namespace GUI {


class Window : NoCopy {
	public:
		enum class WidgetType {
			kButton,
			kTextEdit,
		};

		struct WidgetProcBase {
			using WidgetUnion = Union<Button, TextEdit>;

			template <typename ErrorType>
			WidgetProcBase( const ErrorType* ) {
				throw;
			}
			WidgetProcBase(	const Button* button )
						:	type_( WidgetType::kButton ),
							widget_( button )
			{}
			WidgetProcBase(	const TextEdit* text_edit )
						:	type_( WidgetType::kTextEdit ),
							widget_( text_edit )
			{}

			const WidgetType type_;
			const WidgetUnion widget_;
		};

		struct WidgetRenderer : public WidgetProcBase {
			template <typename Widget>
			WidgetRenderer(	const Widget* widget )
						:	WidgetProcBase( widget )
			{}

			inline void Render() const {
				if ( type_ == WidgetType::kButton ) {
					widget_.get<const Button*>()->Render();
				}
				else if ( type_ == WidgetType::kTextEdit ) {
					widget_.get<const TextEdit*>()->Render();
				}
				else {
					throw;
				}
			}
		};
		
		struct WidgetOnEvent : public WidgetProcBase {
			template <typename Widget>
			WidgetOnEvent(	const Widget* widget )
						:	WidgetProcBase( widget )
			{}

			inline void OnEvent( const MouseEventType& event_type ) const {
				if ( type_ == WidgetType::kButton ) {
					auto object = widget_.get<const Button*>();
					const_cast<Button*>( object )->OnEvent( event_type );
				}
				else if ( type_ == WidgetType::kTextEdit ) {
					auto object = widget_.get<const TextEdit*>();
					const_cast<TextEdit*>( object )->OnEvent( event_type );
				}
				else {
					throw;
				}
			}

			inline sf::FloatRect getBounds() const {
				if ( type_ == WidgetType::kButton ) {
					return widget_.get<const Button*>()->getBounds();
				}
				else if ( type_ == WidgetType::kTextEdit ) {
					return widget_.get<const TextEdit*>()->getBounds();
				}
				throw; // Oh shit.
			}
		};
		
		struct WidgetOnKeyboardEvent : public WidgetProcBase {
			template <typename Widget>
			WidgetOnKeyboardEvent(	const Widget* widget )
								:	WidgetProcBase( widget )
			{}

			inline void OnKeyboardInput( const sf::Event& event ) const {
				if ( type_ == WidgetType::kTextEdit ) {
					auto object = widget_.get<const TextEdit*>();
					const_cast<TextEdit*>( object )->OnKeyboardInput( event );
				}
				else {
					throw;
				}
			}
		};

	public:
		Window(	const sf::VideoMode& video_mode,
				const sf::String& window_name,
				const sf::Uint32& style = sf::Style::Default )
			:	render_window_( video_mode, window_name, style )
		{}

		inline Button* CreateButton(	const sf::Vector2f& position,
										const ButtonThemes& button_themes = {} )
		{
			buttons_.push_back(
				std::make_unique<Button>(
					render_window_,
					position,
					button_themes
				)
			);
			auto object = buttons_.back().get();
			render_procedures_.push_back( WidgetRenderer( object ) );
			on_event_procedures_.push_back( WidgetOnEvent( object ) );
			return object;
		}

		inline TextEdit* CreateTextEdit(
			const sf::Vector2f& position,
			const TextEdit::TextEditThemes& text_edit_themes = TextEdit::DefaultThemes() )
		{
			text_edits_.push_back(
				std::make_unique<TextEdit>(
					render_window_,
					position,
					text_edit_themes
				)
			);
			auto object = text_edits_.back().get();
			render_procedures_.push_back( WidgetRenderer( object ) );
			on_event_procedures_.push_back( WidgetOnEvent( object ) );
			on_keyboard_input_procedures_.push_back( WidgetOnKeyboardEvent( object ) );
			return object;
		}

		inline void OnMouseMoved() {
			widget_hovered_ = WidgetHovered();

			for ( const auto& widget : on_event_procedures_ ) {
				if ( &widget == widget_hovered_ ) {
					widget_hovered_->OnEvent( MouseEventType::kOnHover );
				}
				else {
					widget.OnEvent( MouseEventType::kIdle );
				}
			}
		}

		inline void OnMouseButtonPressed() {
			widget_pressed_ = widget_hovered_;
			
			if (	widget_selected_ &&
					widget_pressed_ != widget_selected_ )
			{
				widget_selected_->OnEvent( MouseEventType::kOnUnselect );
				widget_selected_ = nullptr;
			}

			for ( auto& widget : on_event_procedures_ ) {
				if ( &widget == widget_hovered_ ) {
					widget.OnEvent( MouseEventType::kOnDrag );
				}
				else {
					widget.OnEvent( MouseEventType::kIdle );
				}
			}
		}

		inline void OnMouseButtonReleased() {
			if ( !widget_pressed_ ) return;
			if ( const auto widget_currently_hovered = WidgetHovered() ) {
				if ( widget_currently_hovered == widget_pressed_ ) {
					widget_currently_hovered->OnEvent( MouseEventType::kOnSelect );
					widget_selected_ = widget_currently_hovered;
				}
			}
			widget_pressed_ = nullptr;
		}

		inline void OnKeyboardInput( const sf::Event& event ) {
			for ( auto& widget : on_keyboard_input_procedures_ ) {
				widget.OnKeyboardInput( event );
			}
		}

		constexpr sf::RenderWindow& getRenderWindow() {
			return render_window_;
		}

		void Render() {
			for ( const auto& renderer : render_procedures_ ) {
				renderer.Render();
			}
		}

	private:
		inline const WidgetOnEvent* WidgetHovered() {
			const sf::Vector2f mouse = static_cast<sf::Vector2f>(
				sf::Mouse::getPosition( render_window_ )
			);
			for ( const auto& widget : on_event_procedures_ ) {
				if ( widget.getBounds().contains( mouse ) ) {
					return &widget;
				}
			}
			return nullptr;
		}

		sf::RenderWindow render_window_;
		
		const WidgetOnEvent* widget_hovered_ = nullptr;
		const WidgetOnEvent* widget_selected_ = nullptr;
		const WidgetOnEvent* widget_pressed_ = nullptr;
		
		std::vector<std::unique_ptr<Button>> buttons_;
		std::vector<std::unique_ptr<TextEdit>> text_edits_;

		std::vector<WidgetRenderer> render_procedures_;
		std::vector<WidgetOnEvent> on_event_procedures_;
		std::vector<WidgetOnKeyboardEvent> on_keyboard_input_procedures_;
};


};