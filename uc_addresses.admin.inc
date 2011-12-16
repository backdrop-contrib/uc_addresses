<?php
/**
 * @file
 * Admin settings for Ubercart Addresses
 */

// -----------------------------------------------------------------------------
// MODULE SETTINGS
// -----------------------------------------------------------------------------

/**
 * Display the addresses settings overview.
 * @return string
 */
function uc_addresses_settings_overview() {
  $summaries = array();
  $summaries = array_merge($summaries, summarize_child_form_pages('admin/store/settings/addresses/edit', FALSE, TRUE));

  // Theme it all up in a summaries overview.
  return theme('summary_overview', $summaries);
}

/**
 * Module settings form
 * @return array
 */
function uc_addresses_settings_form() {
  $form['uc_addresses'] = array(
    '#type' => 'fieldset',
    '#title' => t('Ubercart Addresses settings'),
    '#summary callback' => 'summarize_form',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['uc_addresses']['uc_addresses_use_address_name'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use address nicknames'),
    '#description' => t('When checked, users can optionally give an address a name (e.g. "work", "mary", "joe").'),
    '#default_value' => variable_get('uc_addresses_use_address_name', TRUE),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('Users can give addresses a nickname.'),
      t('Users can not give addresses a nickname.'),
    ),
  );
  $form['uc_addresses']['uc_addresses_use_default_billing'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use default billing addresses'),
    '#description' => t('Note that addresses marked as default billing cannot be deleted.'),
    '#default_value' => variable_get('uc_addresses_use_default_billing', TRUE),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('Users can mark an address as default billing.'),
      t('Users can not mark an address as default billing.'),
    ),
  );
  $form['uc_addresses']['uc_addresses_use_default_shipping'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use default shipping addresses'),
    '#description' => t('Note that addresses marked as default shipping cannot be deleted.'),
    '#default_value' => variable_get('uc_addresses_use_default_shipping', TRUE),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('Users can mark an address as default shipping.'),
      t('Users can not mark an address as default shipping.'),
    ),
  );

  $form['checkout'] = array(
    '#type' => 'fieldset',
    '#title' => t('Checkout settings'),
    '#summary callback' => 'summarize_form',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['checkout']['uc_addresses_default_billing_address'] = array(
    '#type' => 'checkbox',
    '#title' => t('Automatically fill in the billing address with the user\'s default billing address.'),
    '#description' => t('Note that %use-default-setting must be activated for this setting to have effect.', array('%use-default-setting' => $form['uc_addresses']['uc_addresses_use_default_billing']['#title'])),
    '#default_value' => variable_get('uc_addresses_default_billing_address', TRUE),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('At checkout the billing address will be automatically filled with the user\'s default billing address.'),
      t('The customer must enter or choose a billing address.'),
    ),
  );
  $form['checkout']['uc_addresses_default_shipping_address'] = array(
    '#type' => 'checkbox',
    '#title' => t('Automatically fill in the delivery address with the user\'s default shipping address.'),
    '#description' => t('Note that %use-default-setting must be activated for this setting to have effect.', array('%use-default-setting' => $form['uc_addresses']['uc_addresses_use_default_shipping']['#title'])),
    '#default_value' => variable_get('uc_addresses_default_shipping_address', TRUE),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('At checkout the delivery address will be automatically filled with the user\'s default shipping address.'),
      t('The customer must enter or choose a delivery address.'),
    ),
  );

  $form['user'] = array(
    '#type' => 'fieldset',
    '#title' => t('User settings'),
    '#summary callback' => 'summarize_form',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['user']['uc_addresses_require_address'] = array(
    '#type' => 'checkbox',
    '#title' => t('Require that an address be entered during registration'),
    '#default_value' => variable_get('uc_addresses_require_address', TRUE),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('The customer must enter an address when registering.'),
      t('The customer does not enter an address when registering.'),
    ),
  );
  $form['user']['uc_addresses_require_address_admin'] = array(
    '#type' => 'checkbox',
    '#title' => t('Require admins to enter an address upon user creation'),
    '#description' => t('When checked, users with the permission %administer-users need to enter an address when they create a new user.', array('%administer-users' => t('administer users'))),
    '#default_value' => variable_get('uc_addresses_require_address_admin', TRUE),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('Admins are required to enter an address when they create a new user.'),
      t("Admins don't have to enter an address when they create a new user."),
    ),
  );

  return system_settings_form($form);
}

// -----------------------------------------------------------------------------
// ADDRESS FORMAT SETTINGS
// -----------------------------------------------------------------------------

/**
 * Lists all enabled addresses were address formats
 * may be set
 * @param int $country_id
 * @return string
 */
function uc_addresses_country_formats_page($country_id = NULL) {
  if ($country_id) {
    // Return address display form for this country
    return drupal_get_form('uc_addresses_country_formats_form', $country_id);
  }

  // List all countries
  $header = array(
    t('Country'),
    t('Status'),
  );

  $rows = array();
  $path = 'admin/store/settings/countries/edit/uc_addresses_formats';
  $result = db_query("SELECT country_id, country_name FROM {uc_countries} ORDER BY country_name ASC");
  while ($country = db_fetch_object($result)) {
    $rows[] = array(
      l($country->country_name, $path . '/' . $country->country_id),
      (variable_get('uc_addresses_address_format_' . $country->country_id, NULL) ? t('Overridden') : t('Ubercart default')),
    );
  }

  return theme('table', $header, $rows);
}

/**
 * Form for address format
 * @param int $country_id
 * @return array
 */
function uc_addresses_country_formats_form($form_state, $country_id) {
  // Get country
  $result = db_query("SELECT country_name FROM {uc_countries} WHERE country_id = %d", $country_id);
  $country = db_fetch_object($result);
  if (!isset($country->country_name)) {
    return;
  }

  $form = array();
  $form['uc_addresses_address_format_' . $country_id] = array(
    '#type' => 'textarea',
    '#title' => t('Address format for @country', array('@country' => $country->country_name)),
    '#default_value' => uc_addresses_get_address_format($country_id),
    '#description' => t('Uses tokens to format an address for this country.'),
    '#rows' => 6,
  );

  $form['token_help'] = array(
    '#type' => 'fieldset',
    '#title' => t('Replacement patterns'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#children' => theme('token_tree', array('uc_addresses')),
  );

  $form = system_settings_form($form);
  return $form;
}
