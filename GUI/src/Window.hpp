#pragma once

#include <memory>
#include <vector>

#include "sfml_includes.hpp"

#include "TextEdit.hpp"

#pragma warning( disable : 4706 ) // For conditional assignment: "if ( int* i = getI() ) {}"


namespace GUI {
	
	

namespace Proto2 {
		
	struct WidgetProcedureBase {};

	template <	typename Widget,
				typename _WidgetP,
				typename ...Args
	>
	struct WidgetProcedure final : public WidgetProcedureBase {
		using WidgetP = _WidgetP;
		using Callback = void(*)( WidgetP, Args... );
		using Derived = WidgetProcedure<Widget, WidgetP, Args...>;

		explicit WidgetProcedure(	WidgetP widget, Callback callback )
								:	widget_( widget ), callback_( callback )
		{}
		
		static Derived* Cast( WidgetProcedureBase* base ) {
			return static_cast<Derived*>( base );
		}
		static const Derived* Cast( const WidgetProcedureBase* base ) {
			return static_cast<const Derived*>( base );
		}
		inline void Invoke( Args... args ) const {
			callback_(
				widget_,
				args...
			);
		}

		WidgetP widget_;
		Callback callback_;
	};

};

class Window : NoCopy {
	private:
		/*template <typename Widget>
		using Renderer = Proto2::WidgetProcedure<
			Widget,
			const Widget*
		>;*/
	public:
		enum class WidgetType {
			kButton,
			kTextEdit,
		};

		struct WidgetProcBase {
			using WidgetUnion = Union<Button, TextEdit>;

			explicit WidgetProcBase(	const Button* button )
									:	type_( WidgetType::kButton ),
										widget_( button )
			{}
			explicit WidgetProcBase(	const TextEdit* text_edit )
									:	type_( WidgetType::kTextEdit ),
										widget_( text_edit )
			{}
			bool operator==( const WidgetProcBase* other ) const {
				return	( other ) &&
						( type_ == other->type_ ) &&
						( widget_ == other->widget_ );
			}
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
			:	render_window_( video_mode, window_name, ValidateStyle( style ) )
		{}
		
		template <typename Widget>
		void MakeHoverable( const Widget* widget ) {
			on_event_listeners_[ MouseEventType::kOnHover ].push_back(
				WidgetOnEvent( widget )
			);
		}
		
		template <typename Widget>
		void MakeSelectable( const Widget* widget ) {
			on_event_listeners_[ MouseEventType::kOnHover ].push_back(
				WidgetOnEvent( widget )
			);
			on_event_listeners_[ MouseEventType::kOnSelect ].push_back(
				WidgetOnEvent( widget )
			);
		}
		
		template <typename Widget>
		void MakeEditable( const Widget* widget ) {
			MakeSelectable( widget );
			on_keyboard_input_procedures_.push_back(
				WidgetOnKeyboardEvent( widget )
			);
		}
		
		// template <typename Widget>
		template <typename Widget, typename Derived = WidgetWrapper<Widget>>
		Widget* CreateWidget(	const sf::Vector2f& position,
								const ButtonThemes::EventsToThemes& events_to_themes )
		{
			widgets_.push_back(
				std::make_unique<Derived>(
					WidgetType::kButton,
					render_window_,
					position,
					events_to_themes
				)
			);
			auto widget_base = widgets_.back().get();
			auto widget = Derived::Cast( widget_base )->getWidget();
			// render_procedures_.push_back( WidgetRenderer( widget ) );


			
			using Renderer = Proto2::WidgetProcedure<
				Widget,
				const Widget*
			>;
			using WidgetP = Renderer::WidgetP;
			using Callback = Renderer::Callback;
			
			const Widget* widget_const = Derived::Cast( widget_base )->getWidget();

			Renderer renderer(
				widget_const,
				[]( WidgetP widget ) {
					widget->Render();
				}
			);

			render_procedures_2_.push_back(
				std::make_unique<Renderer>( renderer )
			);


			return widget;
		}

		inline void OnMouseMoved() {
			auto currently_hovered = WidgetHovered();
			if ( !currently_hovered && !widget_hovered_ ) {
				return;
			}
			if ( currently_hovered == widget_hovered_ ) {
				// Ongoing hover.
				currently_hovered->OnEvent( MouseEventType::kOnHover );
			}
			else if ( currently_hovered && !widget_hovered_ ) {
				// First hover.
				currently_hovered->OnEvent( MouseEventType::kOnHover );
			}
			else if ( !currently_hovered && widget_hovered_ ) {
				// First unhover.
				widget_hovered_->OnEvent( MouseEventType::kIdle );
			}
			widget_hovered_ = currently_hovered;
		}

		inline void OnMouseButtonPressed() {
			widget_pressed_ = widget_hovered_;
			
			if (	widget_selected_ &&
					widget_pressed_ != widget_selected_ )
			{
				// std::cout << "[Window::OnMouseButtonPressed] UnSelect\n";
				widget_selected_->OnEvent( MouseEventType::kOnUnSelect );
				widget_selected_ = nullptr;
			}
			
			// std::wcout << on_event_listeners_[ MouseEventType::kOnSelect ].size() << L'\n';
			for ( const auto& widget : on_event_listeners_[ MouseEventType::kOnSelect ] ) {
				if ( widget == widget_hovered_ ) {
					// std::cout << "OnDrag\n";
					widget.OnEvent( MouseEventType::kOnDrag );
				}
				else {
					// std::cout << "Idle\n";
					widget.OnEvent( MouseEventType::kIdle );
				}
			}
		}

		inline void OnMouseButtonReleased() {
			if ( !widget_pressed_ ) return;
			// std::wcout << "widget_pressed_\n";
			if ( const auto widget_currently_hovered = WidgetHovered() ) {
				// std::wcout << "widget_currently_hovered = WidgetHovered()\n";
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

			using BRenderer = Proto2::WidgetProcedure<
				Button,
				const Button*
			>;
			for ( const auto& renderer : render_procedures_2_ ) {
				// TODO:: Why this cast does not fail? (From TextEdit* to Button*, no inheritance)
				BRenderer::Cast( renderer.get() )->Invoke();
			}
		}

	private:
		inline const WidgetOnEvent* WidgetHovered() {
			const auto mouse = static_cast<sf::Vector2f>(
				sf::Mouse::getPosition( render_window_ )
			);
			for ( const auto& widget : on_event_listeners_[ MouseEventType::kOnHover ] ) {
				if ( widget.getBounds().contains( mouse ) ) {
					return &widget;
				}
			}
			return nullptr;
		}

		inline bool ListensEvent(	const GUI::MouseEventType event,
									const ButtonThemes::EventsToThemes& events_to_themes )
		{
			const auto search_on_hover = events_to_themes.find( event );
			return ( search_on_hover != std::end( events_to_themes ) );
		}

		inline const sf::Uint32& ValidateStyle( const sf::Uint32& style ) {
			const bool valid =	( style == sf::Style::None ) ||
								( style == sf::Style::Titlebar ) ||
								( style == sf::Style::Resize ) ||
								( style == sf::Style::Close ) ||
								( style == sf::Style::Fullscreen ) ||
								( style == sf::Style::Default );
			if ( !valid ) {
				auto msg = "Unknown style: " + std::to_string( style );
				throw std::runtime_error( msg );
			}
			return style;
		}

		sf::RenderWindow render_window_;
		
		const WidgetOnEvent* widget_hovered_ = nullptr;
		const WidgetOnEvent* widget_selected_ = nullptr;
		const WidgetOnEvent* widget_pressed_ = nullptr;
		
		class WidgetWrapperBase {
			public:
				WidgetWrapperBase( const WidgetType type ) : type_( type ) {}
				const WidgetType type_;
		};
		template <typename Widget>
		class WidgetWrapper final : public WidgetWrapperBase, NoCopy {
			public:
				using Derived = WidgetWrapper<Widget>;

				template <typename ...Args>
				WidgetWrapper( const WidgetType type, Args&&... args )
							:	WidgetWrapperBase( type ),
								widget_( std::forward<Args>( args )... )
				{}
				
				static Derived* Cast( WidgetWrapperBase* base ) {
					return static_cast<Derived*>( base );
				}
				static const Derived* Cast( const WidgetWrapperBase* base ) {
					return static_cast<const Derived*>( base );
				}
				
				Widget* getWidget() {
					return &widget_;
				}
				const Widget* getWidget() const {
					return &widget_;
				}

			private:
				Widget widget_;
		};

		std::vector<std::unique_ptr<WidgetWrapperBase>> widgets_;

		std::vector<WidgetRenderer> render_procedures_;
		std::vector<std::unique_ptr<Proto2::WidgetProcedureBase>> render_procedures_2_;

		std::map<MouseEventType, std::vector<WidgetOnEvent>> on_event_listeners_;
		std::vector<WidgetOnKeyboardEvent> on_keyboard_input_procedures_;
};

};