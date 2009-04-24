<?php

/*******************************************************************************
 * Callback Functions, Forms, and Tables
 ******************************************************************************/

/**
 * Display the addresses settings overview.
 */
function uc_addresses_settings_overview() {
  $summaries = array();
  $summaries = array_merge($summaries, summarize_child_form_pages('admin/store/settings/addresses/edit', FALSE, TRUE));

  // Theme it all up in a summaries overview.
  return theme('summary_overview', $summaries);

}

function uc_addresses_settings_form() {
  $form['user'] = array(
    '#type' => 'fieldset',
    '#title' => t('User settings'),
    '#summary callback' => 'summarize_form',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['user']['uc_addresses_default_billing_address'] = array(
    '#type' => 'checkbox',
    '#title' => t('Automatically fill in the billing address with the user\'s default address.'),
    '#default_value' => variable_get('uc_addresses_default_billing_address', TRUE),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('We will automatically fill in the delivery address with the user\'s default address.'),
      t('The customer must enter or choose a delivery address.')),
    );
  $form['user']['uc_addresses_default_delivery_address'] = array(
    '#type' => 'checkbox',
    '#title' => t('Automatically fill in the delivery address with the user\'s default address.'),
    '#default_value' => variable_get('uc_addresses_default_delivery_address', TRUE),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('We will automatically fill in the billing address with the user\'s default address.'),
      t('The customer must enter or choose a billing address.')),
      );
  $form['user']['uc_addresses_require_address'] = array(
    '#type' => 'checkbox',
    '#title' => t('Require that an address be entered during registration'),
    '#default_value' => variable_get('uc_addresses_require_address', TRUE),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('The customer must enter an address when registering'),
      t('The customer does not enter an address when registering.')),
    );

  return system_settings_form($form);
}
