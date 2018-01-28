#pragma once
#include <tuple>
#include <systemd/sd-bus.h>
#include <sdbusplus/server.hpp>

namespace sdbusplus
{
namespace fi
{
namespace iki
{
namespace halo
{
namespace Rego1
{
namespace server
{

class Sensors
{
    public:
        /* Define all of the basic class operations:
         *     Not allowed:
         *         - Default constructor to avoid nullptrs.
         *         - Copy operations due to internal unique_ptr.
         *         - Move operations due to 'this' being registered as the
         *           'context' with sdbus.
         *     Allowed:
         *         - Destructor.
         */
        Sensors() = delete;
        Sensors(const Sensors&) = delete;
        Sensors& operator=(const Sensors&) = delete;
        Sensors(Sensors&&) = delete;
        Sensors& operator=(Sensors&&) = delete;
        virtual ~Sensors() = default;

        /** @brief Constructor to put object onto bus at a dbus path.
         *  @param[in] bus - Bus to attach to.
         *  @param[in] path - Path to attach at.
         */
        Sensors(bus::bus& bus, const char* path);



        /** @brief Implementation for ReadAll
         *  Reads all Rego600 sensors
         *
         *  @return values[std::vector<std::tuple<std::string, sdbusplus::message::variant<bool, double>>>] - An array of sensor names and values.
         */
        virtual std::vector<std::tuple<std::string, sdbusplus::message::variant<bool, double>>> readAll(
            ) = 0;




    private:

        /** @brief sd-bus callback for ReadAll
         */
        static int _callback_ReadAll(
            sd_bus_message*, void*, sd_bus_error*);


        static constexpr auto _interface = "fi.iki.halo.Rego1.Sensors";
        static const vtable::vtable_t _vtable[];
        sdbusplus::server::interface::interface
                _fi_iki_halo_Rego1_Sensors_interface;


};


} // namespace server
} // namespace Rego1
} // namespace halo
} // namespace iki
} // namespace fi
} // namespace sdbusplus

