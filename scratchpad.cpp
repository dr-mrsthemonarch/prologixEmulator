//
// Created by Kyle on 18.06.24.
//
#include "order_book_component.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/dom/elements.hpp"
#include "data_utils.hpp"  // Include the utility header
#include <random>

using namespace ftxui;

// Function to create order book table
ftxui::Element OrderBookTable(const std::vector<OrderBookComponent::Order>& orders, bool is_bid) {
  ftxui::Elements rows;
  for (const auto& order : orders) {
    auto price_text = text(FormatFloat(order.price)) | flex | (is_bid ? color(Color::Green) : color(Color::Red));
    auto size_text = text(FormatFloat(order.size)) | flex;

    if (order.is_user_order) {
      price_text = price_text | bold;
      size_text = size_text | bold | bgcolor(Color::BlueLight);
    }

    rows.push_back(hbox({
      price_text,
      separator(),
      size_text,
    }) | xflex);
  }
  return vbox(std::move(rows));
}

OrderBookComponent::OrderBookComponent()
  : last_update_time_(std::chrono::steady_clock::now()),
    random_engine_(std::random_device{}()),
    price_distribution_(45000, 55000),
    volume_distribution_(1, 5) {

  InitializeTable();

  order_book_component_ = Container::Vertical({
      Renderer([&] {
        return text("Order Book") | bold | hcenter | size(HEIGHT, LESS_THAN, 1);
      }),
      Renderer(table_container_, [&] {
        return table_container_->Render() | vscroll_indicator | frame | flex;
      })
  });
}

void OrderBookComponent::InitializeTable() {
  table_container_ = Container::Vertical({});
  bids_ = GenerateSimulatedOrders<OrderBookComponent::Order>(true, 50, 50000, 100);
  asks_ = GenerateSimulatedOrders<OrderBookComponent::Order>(false, 50, 50500, 100);

  // Create rows for bids and asks
  for (int i = 0; i < 50; ++i) {
    auto row = Make<TableRow>(i, &bids_, &asks_);
    table_container_->Add(row);
    rows_.push_back(row);
  }
}

ftxui::Component OrderBookComponent::GetRenderer() {
  return order_book_component_;
}

void OrderBookComponent::Update() {
  // Periodic update to change row values
  UpdateRowValuesIfNeeded();
}

void OrderBookComponent::UpdateRowValuesIfNeeded() {
  auto now = std::chrono::steady_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_update_time_).count();
  if (elapsed >= 5) {
    bids_ = GenerateSimulatedOrders<OrderBookComponent::Order>(true, 50, 50000, 100);
    asks_ = GenerateSimulatedOrders<OrderBookComponent::Order>(false, 50, 50500, 100);
    last_update_time_ = now; // Reset the timestamp
  }
}

// Definition of TableRow
OrderBookComponent::TableRow::TableRow(int index, std::vector<Order>* bids, std::vector<Order>* asks)
  : index_(index), bids_(bids), asks_(asks) {}

ftxui::Element OrderBookComponent::TableRow::Render() {
  const auto& bid_order = (*bids_)[index_];
  const auto& ask_order = (*asks_)[index_];

  auto bid_price = text(FormatFloat(bid_order.price)) | color(Color::Green) | flex;
  auto bid_volume = text(FormatFloat(bid_order.size)) | color(Color::Green) | flex;
  auto ask_price = text(FormatFloat(ask_order.price)) | color(Color::Red) | flex;
  auto ask_volume = text(FormatFloat(ask_order.size)) | color(Color::Red) | flex;

  if (bid_order.is_user_order) {
    bid_price = bid_price | bold;
    bid_volume = bid_volume | bold | bgcolor(Color::BlueLight);
  }

  if (ask_order.is_user_order) {
    ask_price = ask_price | bold;
    ask_volume = ask_volume | bold | bgcolor(Color::BlueLight);
  }

  auto element = hbox({
    bid_price | size(WIDTH, LESS_THAN, 15) | flex,
    separator() | color(Color::GrayDark),
    bid_volume | size(WIDTH, LESS_THAN, 15) | flex,
    filler(),
    ask_price | size(WIDTH, LESS_THAN, 15) | flex,
    separator() | color(Color::GrayDark),
    ask_volume | size(WIDTH, LESS_THAN, 15) | flex
  }) | xflex;

  if (Focused()) {
    element = focus(element);
    element |= inverted;
  } else if (Active()) {
    element = select(element);
    element |= inverted;
  }

  return element | size(HEIGHT, EQUAL, 1);
}

bool OrderBookComponent::TableRow::Focusable() const {
  return true;
}

//=========

#include <thread>
#include <atomic>
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

#include "htop_component.hpp"
#include "flags_component.hpp"
#include "order_book_component.hpp"
#include "bar_chart_component.hpp"

using namespace ftxui;

int main() {
  auto screen = ScreenInteractive::Fullscreen();

  // Flags
  FlagsComponent flags_component;
  auto flags_renderer = flags_component.GetRenderer();

  // Htop
  HtopComponent htop_component;

  // Order Book
  OrderBookComponent order_book_component;
  auto order_book_renderer = order_book_component.GetRenderer();

  // Bar Chart Component
  BarChartComponent bar_chart_component;

  int tab_index = 0;
  std::vector<std::string> tab_entries = {
      "flags", "orderbook", "barchart", "chart 2"
  };
  auto tab_selection =
      Menu(&tab_entries, &tab_index, MenuOption::HorizontalAnimated());
  auto tab_content = Container::Tab(
      {
          flags_renderer,
          order_book_renderer,
          Renderer([&] { return bar_chart_component.Render(); }),
          Renderer([&] { return htop_component.Render(); }),
      },
      &tab_index);

  auto exit_button = Button(
      "Exit", [&] { screen.Exit(); }, ButtonOption::Animated());

  auto main_container = Container::Vertical({
      Container::Horizontal({
          tab_selection,
          exit_button,
      }),
      tab_content,
  });

  auto main_renderer = Renderer(main_container, [&] {
    return vbox({
        text("FTXUI Terminal TUI Demo") | bold | hcenter,
        hbox({
            tab_selection->Render() | flex,
            exit_button->Render(),
        }),
        tab_content->Render() | flex,
    });
  });

  std::atomic<bool> refresh_ui_continue = true;
  std::thread refresh_ui([&] {
    while (refresh_ui_continue) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(0.05s);
      screen.Post([&] {
        flags_component.Update();
        order_book_component.Update();
        bar_chart_component.Update();
        htop_component.Update();
      });
      screen.Post(Event::Custom);
    }
  });

  screen.Loop(main_renderer);
  refresh_ui_continue = false;
  refresh_ui.join();

  return 0;
}

auto output = Renderer([&] {
    Elements children = {
        text("Server status, connections and outputs are printed here."),
        separator() | color(Color::Yellow),
    };
    for (size_t i = std::max(0, (int) sharedVec.vec.size() - 9); i < sharedVec.vec.size(); ++i) {
        std::lock_guard<std::mutex> lock(sharedVec.vecMutex);
        children.push_back(text(sharedVec.vec[i]));
    }
    return window(text("Output"), vbox(children) | color(Color::Yellow4) | flex) | color(Color::Yellow4);
});