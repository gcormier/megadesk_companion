import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import uart
from esphome.components import sensor
from esphome.const import DEVICE_CLASS_DISTANCE, STATE_CLASS_MEASUREMENT, UNIT_METER, CONF_ID, CONF_INDEX

CONF_MAX_HEIGHT = "max_height"
CONF_MIN_HEIGHT = "min_height"
CONF_RAW_HEIGHT = "raw_height"
CONF_HEIGHT_CM = "height_cm"
CONF_HEIGHT_INCHES = "height_inches"
CODEOWNERS = ["@swoboda1337"]

DEPENDENCIES = ['uart']

megadesk_ns = cg.esphome_ns.namespace('megadesk')

MegaDesk = megadesk_ns.class_('MegaDesk', cg.Component, sensor.Sensor, uart.UARTDevice)

CONFIG_SCHEMA = uart.UART_DEVICE_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(MegaDesk),
        cv.Optional(CONF_MAX_HEIGHT): sensor.sensor_schema(
            # unit_of_measurement=UNIT_METER,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_DISTANCE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_MIN_HEIGHT): sensor.sensor_schema(
            # unit_of_measurement=UNIT_METER,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_DISTANCE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_RAW_HEIGHT): sensor.sensor_schema(
            # unit_of_measurement=UNIT_METER,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_DISTANCE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_HEIGHT_CM): sensor.sensor_schema(
            unit_of_measurement=UNIT_METER,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_DISTANCE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_HEIGHT_INCHES): sensor.sensor_schema(
            unit_of_measurement='inches',
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_DISTANCE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    if CONF_MAX_HEIGHT in config:
        sens = await sensor.new_sensor(config[CONF_MAX_HEIGHT])
        cg.add(var.set_max_height_sensor(sens))
    if CONF_MIN_HEIGHT in config:
        sens = await sensor.new_sensor(config[CONF_MIN_HEIGHT])
        cg.add(var.set_min_height_sensor(sens))
    if CONF_RAW_HEIGHT in config:
        sens = await sensor.new_sensor(config[CONF_RAW_HEIGHT])
        cg.add(var.set_raw_height_sensor(sens))
    if CONF_HEIGHT_CM in config:
        sens = await sensor.new_sensor(config[CONF_HEIGHT_CM])
        cg.add(var.set_height_cm_sensor(sens))
    if CONF_HEIGHT_INCHES in config:
        sens = await sensor.new_sensor(config[CONF_HEIGHT_INCHES])
        cg.add(var.set_height_inches_sensor(sens))
