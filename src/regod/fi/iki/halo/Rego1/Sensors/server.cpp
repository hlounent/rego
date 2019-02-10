#include <algorithm>
#include <sdbusplus/server.hpp>
#include <sdbusplus/exception.hpp>
#include <fi/iki/halo/Rego1/Sensors/server.hpp>


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

Sensors::Sensors(bus::bus& bus, const char* path)
        : _fi_iki_halo_Rego1_Sensors_interface(
                bus, path, _interface, _vtable, this)
{
}


int Sensors::_callback_ReadAll(
        sd_bus_message* msg, void* context, sd_bus_error* error)
{
    using sdbusplus::server::binding::details::convertForMessage;

    try
    {
        auto m = message::message(msg);
#if 0
        {
            auto tbus = m.get_bus();
            sdbusplus::server::transaction::Transaction t(tbus, m);
            sdbusplus::server::transaction::set_id
                (std::hash<sdbusplus::server::transaction::Transaction>{}(t));
        }
#endif


        auto o = static_cast<Sensors*>(context);
        auto r =         o->readAll();

        auto reply = m.new_method_return();
        reply.append(convertForMessage(std::move(r)));

        reply.method_return();
    }
    catch(sdbusplus::internal_exception_t& e)
    {
        sd_bus_error_set_const(error, e.name(), e.description());
        return -EINVAL;
    }

    return true;
}

namespace details
{
namespace Sensors
{
static const auto _param_ReadAll =
        utility::tuple_to_array(std::make_tuple('\0'));
static const auto _return_ReadAll =
        utility::tuple_to_array(message::types::type_id<
                std::vector<std::tuple<std::string, sdbusplus::message::variant<bool, double>>>>());
}
}




const vtable::vtable_t Sensors::_vtable[] = {
    vtable::start(),

    vtable::method("ReadAll",
                   details::Sensors::_param_ReadAll
                        .data(),
                   details::Sensors::_return_ReadAll
                        .data(),
                   _callback_ReadAll,
                   SD_BUS_VTABLE_UNPRIVILEGED
                   ),
    vtable::end()
};

} // namespace server
} // namespace Rego1
} // namespace halo
} // namespace iki
} // namespace fi
} // namespace sdbusplus

