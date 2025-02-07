/*
 * Copyright 2024 Vadym Omelchenko.
 *
 * This file is part of the mavros package and subject to the license terms
 * in the top-level LICENSE file of the mavros repository.
 * https://github.com/mavlink/mavros/tree/master/LICENSE.md
 */
/**
 * @brief Direction Finder plugin
 * @file direction_finder.cpp
 * @author Vadym Omelchenko <omelchenkovadym@gmail.com>
 *
 * @addtogroup plugin
 * @{
 */

#include "rcpputils/asserts.hpp"
#include "mavros/mavros_uas.hpp"
#include "mavros/plugin.hpp"
#include "mavros/plugin_filter.hpp"
#include "mavros_msgs/msg/radio_signal.hpp"

namespace mavros
{
namespace extra_plugins
{
using namespace std::placeholders;      // NOLINT

/**
 * @brief Direction Finder plugin.
 * @plugin direction_finder
 *
 *
 * Sends info about radio signal to the FCU and
 * publishes data that comes from FCU.
 *
 * @see send_direction_finder()	transforming and sending radio signal to fcu
 * @see handle_direction_finder()	receiving and transforming radio signal from fcu
 */
class DirectionFinderPlugin : public plugin::Plugin
{
public:
  explicit DirectionFinderPlugin(plugin::UASPtr uas_)
  : Plugin(uas_, "direction_finder")
  {
    direction_finder_pub = node->create_publisher<mavros_msgs::msg::RadioSignal>("~/in", 10);

    // subscribers
    direction_finder_sub =
      node->create_subscription<mavros_msgs::msg::RadioSignal>(
      "~/out", 1,
      std::bind(&DirectionFinderPlugin::send_direction_finder, this, _1));
  }

  Subscriptions get_subscriptions() override
  {
    return {
      make_handler(&DirectionFinderPlugin::handle_direction_finder),
    };
  }

private:
  rclcpp::Publisher<mavros_msgs::msg::RadioSignal>::SharedPtr direction_finder_pub;
  rclcpp::Subscription<mavros_msgs::msg::RadioSignal>::SharedPtr direction_finder_sub;

  void handle_direction_finder(
    const mavlink::mavlink_message_t * msg [[maybe_unused]],
    mavlink::common::msg::RADIO_SIGNAL & radio_signal,
    plugin::filter::SystemAndOk filter [[maybe_unused]])
  {
    auto signal = mavros_msgs::msg::RadioSignal();

    signal.header.stamp = node->now();
    signal.rate = radio_signal.rate;
    signal.heading = radio_signal.heading;
    signal.level = radio_signal.level;

    direction_finder_pub->publish(signal);
  }

  void send_direction_finder(const mavros_msgs::msg::RadioSignal::SharedPtr data)
  {
    mavlink::common::msg::RADIO_SIGNAL msg {};

    RCLCPP_DEBUG_STREAM(
      get_logger(),
      "DRFN: output: rate:" << std::endl << data->rate);
    RCLCPP_DEBUG_STREAM(
      get_logger(),
      "DRFFN: output: heading" << std::endl << data->heading);
    RCLCPP_DEBUG_STREAM(
      get_logger(),
      "DRFFN: output: level" << std::endl << data->level);

    //msg.header.stamp = data->header.stamp;
    msg.rate = data->rate;
    msg.heading = data->heading;
    msg.level = data->level;

    // send RADIO_SIGNAL msg
    uas->send_message(msg);
  }
};
}       // namespace extra_plugins
}       // namespace mavros

#include <mavros/mavros_plugin_register_macro.hpp>  // NOLINT
MAVROS_PLUGIN_REGISTER(mavros::extra_plugins::DirectionFinderPlugin)
