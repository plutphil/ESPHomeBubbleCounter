import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.const import CONF_ID

interrupt_ns = cg.esphome_ns.namespace("interrupt")
Interrupt = interrupt_ns.class_("Interrupt", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(Interrupt),
    cv.Required("pin"): pins.internal_gpio_input_pin_schema,
})

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    pin = await cg.gpio_pin_expression(config["pin"])
    cg.add(var.set_pin(pin))
    await cg.register_component(var, config)