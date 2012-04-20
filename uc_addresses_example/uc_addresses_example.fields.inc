<?php
/**
 * @file
 * Definitions of fields used in schema's and by Ubercart Addresses.
 *
 * These fields will be added to uc_addresses table and to uc_orders table.
 * The fields will be also registered, so Ubercart Addresses knows about
 * them.
 */

/**
 * Defines which schema tables needs to be altered.
 *
 * A prefix is also defined because in case of the
 * uc_orders table two fields needs to be added:
 * - one for delivery
 * - and one for billing.
 *
 * @return array
 *   The schema's that needs to be altered.
 */
function _uc_addresses_example_schema_tables() {
  return array(
    array(
      'table' => 'uc_addresses',
      'prefix' => '',
    ),
    array(
      'table' => 'uc_orders',
      'prefix' => 'delivery_',
    ),
    array(
      'table' => 'uc_orders',
      'prefix' => 'billing_',
    ),
  );
}

/**
 * Defines which schema fields need to be added
 * to the uc_addresses and uc_orders table.
 *
 * Some of the fields are set to not display by default.
 * These fields should be shown in forms, but it's value
 * should not be shown separately. The idea is that these
 * fields are added to the address format, which can be set
 * at:
 * admin/store/settings/countries/uc_addresses_formats
 *
 * @return array
 */
function _uc_addresses_example_schema_fields() {
  return array(
    'title' => array(
      // Schema
      'description' => t("Person's stating social rank."),
      'type' => 'varchar',
      'length' => 255,
      'not null' => TRUE,
      'default' => '',
      // UC Addresses
      'title' => t('Title'),
      'handler' => 'UcAddressesTextFieldHandler',
      'display_settings' => array(
        'default' => FALSE,
        'register' => TRUE,
        'address_form' => TRUE,
        'checkout_form' => TRUE,
        'order_form' => TRUE,
      ),
      'compare' => TRUE,
    ),
    'surname_prefix' => array(
      // Schema
      'description' => 'The addressee\'s surname prefix',
      'type' => 'varchar',
      'length' => 255,
      'not null' => TRUE,
      'default' => '',
      // UC Addresses
      'title' => t('Surname prefix'),
      'handler' => 'UcAddressesTextFieldHandler',
      'display_settings' => array(
        'default' => FALSE,
        'register' => TRUE,
        'address_form' => TRUE,
        'checkout_form' => TRUE,
        'order_form' => TRUE,
      ),
      'compare' => TRUE,
    ),
    'house_number' => array(
      // Schema
      'description' => 'The addressee\'s house number',
      'type' => 'varchar',
      'length' => 10,
      'not null' => TRUE,
      'default' => '',
      // UC Addresses
      'title' => t('House number'),
      'handler' => 'UcAddressesTextFieldHandler',
      'display_settings' => array(
        'default' => FALSE,
        'register' => TRUE,
        'address_form' => TRUE,
        'checkout_form' => TRUE,
        'order_form' => TRUE,
      ),
      'compare' => TRUE,
    ),
  );
}
