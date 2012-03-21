<?php
/**
 * @file
 * Ubercart callbacks for the checkout- and order panes
 */

// ---------------------------------------------------------------------------
// UBERCART CALLBACKS
// checkout panes
// ---------------------------------------------------------------------------

/**
 * Gets the delivery information.
 *
 * Overrides uc_checkout_pane_delivery().
 *
 * @see uc_addresses_checkout_pane_address()
 */
function uc_addresses_checkout_pane_shipping($op, $order, $form = NULL, &$form_state = NULL) {
  $description = t('Enter your delivery address and information here.');
  $copy = t('My delivery information is the same as my billing information.');
  return uc_addresses_checkout_pane_address('shipping', $op, $order, $form, $form_state, $description, $copy);
}

/**
 * Gets the billing information.
 *
 * Overrides uc_checkout_pane_billing().
 *
 * @see uc_addresses_checkout_pane_address()
 */
function uc_addresses_checkout_pane_billing($op, $order, $form = NULL, &$form_state = NULL) {
  $description = t('Enter your billing address and information here.');
  $copy = t('My billing information is the same as my delivery information.');
  return uc_addresses_checkout_pane_address('billing', $op, $order, $form, $form_state, $description, $copy);
}

/**
 * Generic address pane handler.
 *
 * @param string $type
 *   The address type: billing or shipping
 * @param string $op
 *   The operation that is being performed
 * @param object $order
 *   The Ubercart order
 * @param array $form
 *   The checkout form
 * @param array $form_state
 *   The form state of the checkout form
 * @param string $description
 *   The help text shown above the form
 * @param string $copy
 *   The text shown for the "copy address" checkbox
 *
 * @return mixed
 */
function uc_addresses_checkout_pane_address($type, $op, $order, $form, &$form_state, $description, $copy) {
  global $user;

  if ($type == 'shipping') {
    $other_type = 'billing';
    $pane = 'delivery';
  }
  else {
    $other_type = 'delivery';
    $pane = 'billing';
  }

  // Source pane for "copy address" checkbox.
  static $source;
  if (!isset($source)) {
    $source = $pane;
  }

  switch ($op) {
    case 'view':
      // Tell addres book we want to get multiple addresses
      UcAddressesAddressBook::get($user->uid)->setPerformanceHint(UcAddressesAddressBook::PERF_HINT_LOAD_ALL);

      // Initialize address variable
      $address = NULL;

      // Check if address may be automatically filled in
      $auto_fill_address = (variable_get('uc_addresses_default_' . $type . '_address', TRUE) && variable_get('uc_addresses_use_default_' . $type, TRUE));

      // Get address for order if set
      if (isset($order->uc_addresses[$type])) {
        $address = $order->uc_addresses[$type];
      }
      elseif ($auto_fill_address) {
        // Get default address of type $type
        $address = UcAddressesAddressBook::get($user->uid)->getDefaultAddress($type);
        if ($address) {
          $address = $address->copyAddress(UcAddressesAddressBook::get(0));
        }
      }
      if (!$address) {
        $address = UcAddressesAddress::newAddress();
      }

      // Check if address is copied from source pane
      // (if the "copy address" checkbox was enabled)
      if (
        (isset($address->copy_address) && $address->copy_address)
        || !empty($form_state['values']['panes'][$pane]['copy_address'])
      ) {
        $copy_address_checked = TRUE;
      }
      else {
        $copy_address_checked = FALSE;
      }

      if ($source != $pane) {
        $contents['copy_address'] = array(
          '#type' => 'checkbox',
          '#title' => $copy,
          '#ajax' => array(
            'callback' => 'uc_checkout_pane_address_render',
            'wrapper' => $pane . '-address-pane',
            'progress' => array(
              'type' => 'throbber',
            ),
          ),
          '#default_value' => $copy_address_checked,
        );
      }

      if ($user->uid && $select = uc_addresses_select_addresses($user->uid, 'checkout_form', $pane, $address)) {
        $contents['select_address'] = $select + array(
          '#type' => 'select',
          '#title' => t('Saved addresses'),
          '#ajax' => array(
            'callback' => 'uc_checkout_pane_address_render',
            'wrapper' => $pane . '-address-pane',
            'progress' => array(
              'type' => 'throbber',
            ),
          ),
          '#states' => array(
            'invisible' => array(
              'input[name="panes[' . $pane . '][copy_address]"]' => array('checked' => TRUE),
            ),
          ),
        );
      }

      if (isset($form_state['triggering_element'])) {
        $element = &$form_state['triggering_element'];

        if ($element['#name'] == "panes[$pane][copy_address]") {
          $address_data = &$form_state['values']['panes'][$source];
          foreach ($address_data as $source_fieldname => $value) {
            if (substr($source_fieldname, 0, strlen($source)) == $source) {
              $fieldname = substr($source_fieldname, strlen($source . '_'));
              $target_fieldname = str_replace($source, $pane, $source_fieldname);
              $form_state['input']['panes'][$pane]['address'][$target_fieldname] = $value;
              $order->$target_fieldname = $value;
              if (UcAddressesSchemaAddress::fieldExists($fieldname)) {
                $address->setField($fieldname, $value);
              }
            }
          }
        }

        if ($element['#name'] == "panes[$pane][select_address]") {
          $address_id = $element['#value'];
          $address_source = UcAddressesAddressBook::get($user->uid)->getAddressById($address_id);
          if ($address_source instanceof UcAddressesAddress) {
            $address = $address_source->copyAddress(UcAddressesAddressBook::get(0));
            $address_data = $address->getRawFieldData();
            foreach ($address_data as $field => $value) {
              $form_state['input']['panes'][$pane]['address'][$pane . '_' . $field] = $value;
              $order->{$pane . '_' . $field} = $value;
            }
          }
        }

        // Forget any previous Ajax submissions, as we send new default values.
        unset($form_state['uc_addresses_address']);
      }

      $contents['address'] = array(
        '#type' => 'uc_addresses_address',
        '#uc_addresses_context' => 'checkout_form',
        '#uc_addresses_address' => $address,
        '#key_prefix' => $pane,
        '#hidden' => $copy_address_checked,
        '#prefix' => '<div id="' . $pane . '-address-pane">',
        '#suffix' => '</div>',
      );
/*
      // Form element for debug purposes, displays the address that is taken.
      $contents['address']['display'] = array(
        '#type' => 'item',
        '#title' => t('Address'),
        '#markup' => (string) $address,
        '#weight' => 100,
      );
//*/

      return array('description' => $description, 'contents' => $contents);

    case 'process':
      $address = $form['panes'][$pane]['address']['#uc_addresses_address'];
      $address->op = $op;
      $panes = &$form_state['values']['panes'];

      if ($address->isNew() && !(isset($panes[$pane]['copy_address']) && $panes[$pane]['copy_address'])) {
        // Set flag that this address may be saved to the addressbook
        $address->save_address = TRUE;
      }
      // Set flag for if this address is "copied" or not.
      if (isset($panes[$pane]['copy_address'])) {
        if (!empty($panes[$pane]['copy_address'])) {
          $address->copy_address = TRUE;
        }
        else {
          $address->copy_address = FALSE;
        }
      }

      foreach ($panes[$pane]['address'] as $target_fieldname => $value) {
        if (substr($target_fieldname, 0, strlen($pane)) == $pane) {
          if (!empty($panes[$pane]['copy_address'])) {
            $fieldname = substr($target_fieldname, strlen($pane . '_'));
            $value = $panes[$source]['address'][str_replace($pane, $source, $target_fieldname)];
            if (UcAddressesSchemaAddress::fieldExists($fieldname)) {
              $address->setField($fieldname, $value);
            }
          }
          $order->$target_fieldname = $value;
        }
      }

      // Temporary save address with order
      $order->uc_addresses[$type] = $address;
      // Save address into session
      $_SESSION['uc_addresses_order'][$order->order_id][$type] = serialize($address);
      return TRUE;

    case 'review':
      drupal_add_css(drupal_get_path('module', 'uc_addresses') . '/uc_addresses.css');
      return uc_addresses_preprocess_address($order->uc_addresses[$type], 'checkout_review');
  }
}

// ---------------------------------------------------------------------------
// UBERCART CALLBACKS
// order panes
// @todo Implement!
// ---------------------------------------------------------------------------

/**
 * Callback for the "bill to" pane.
 *
 * Overrides uc_order_pane_bill_to().
 *
 * @see uc_addresses_order_pane_address()
 */
function uc_addresses_order_pane_bill_to($op, $order, &$form = NULL, &$form_state = NULL) {
  return uc_addresses_order_pane_address('billing', $op, $order, $form, $form_state);
}

/**
 * Callback for the "ship to" pane.
 *
 * Overrides uc_order_pane_ship_to().
 *
 * @see uc_addresses_order_pane_address()
 */
function uc_addresses_order_pane_ship_to($op, $order, &$form = NULL, &$form_state = NULL) {
  return uc_addresses_order_pane_address('shipping', $op, $order, $form, $form_state);
}

/**
 * Callback for an address order pane.
 *
 * @param string $type
 *   The address type: billing or shipping
 * @param string $op
 *   The operation that is being performed
 * @param object $order
 *   The Ubercart order
 * @param array $form
 *   The order edit form
 * @param array $form_state
 *   The form state of the order edit form
 *
 * @return mixed
 */
function uc_addresses_order_pane_address($type, $op, $order, &$form = NULL, &$form_state = NULL) {
  if ($type == 'shipping') {
    $other_type = 'billing';
    $uc_type = 'delivery';
  }
  else {
    $other_type = 'delivery';
    $uc_type = 'billing';
  }

  switch ($op) {
    case 'customer':
    case 'view':
      drupal_add_css(drupal_get_path('module', 'uc_addresses') . '/uc_addresses.css');
      $lines = array();
      $values = uc_addresses_preprocess_address($order->uc_addresses[$type], 'order_view');
      foreach ($values as $fieldname => $value) {
        if (isset($value['title']) && $value['title'] != '') {
          $lines[] = '<strong>' . $value['title'] . '</strong>: ' . $value['data'];
        }
        else {
          $lines[] = $value['data'] . '<br />';
        }
      }
      $build = array('#markup' => implode('<br />', $lines));
      return $build;

    case 'edit-form':
      drupal_add_js(drupal_get_path('module', 'uc_addresses') . '/uc_addresses.js');
      drupal_add_js(drupal_get_path('module', 'uc_order') . '/uc_order.js');
      // Initialize pane description
      switch ($type) {
        case 'shipping':
          $description = t("Modify 'Ship to' information");
          break;
        case 'billing':
          $description = t("Modify 'Bill to' information");
          break;
      }

      // Get address for order if set
      if (isset($order->uc_addresses[$type])) {
        $address = $order->uc_addresses[$type];
      }

      // View the address form
      $form[$uc_type] = array(
        '#type' => 'uc_addresses_address',
        '#uc_addresses_context' => 'order_form',
        '#key_prefix' => $uc_type,
      );
      if (!$address) {
        $address = UcAddressesAddress::newAddress();
      }
      // Add address to the form
      $form[$uc_type]['#uc_addresses_address'] = $address;
      return $form;

    case 'edit-title':
      switch ($type) {
        case 'shipping':
          $copy_text = t("Copy billing information.");
          break;
        case 'billing':
          $copy_text = t("Copy shipping information.");
          break;
      }
      $output = ' <img src="' . base_path() . drupal_get_path('module', 'uc_store')
        . '/images/address_book.gif" alt="' . t('Select from address book.') . '" '
        . 'title="' . t('Select from address book.') . '" onclick="load_address_select(' . $form['order_uid']['#value'] . ', \'#' . $uc_type . '_address_select\', \'' . $uc_type . '\');" '
        . 'style="position: relative; top: 2px; cursor: pointer;" />';
      $output .= ' <img src="' . base_path() . drupal_get_path('module', 'uc_store')
        . '/images/copy.gif" alt="' . $copy_text . '" title="'
        . $copy_text . '" onclick="uc_addresses_copy_address_order(\'' . $other_type . '\', \'' . $uc_type . '\');" '
        . 'style="position: relative; top: 2px; cursor: pointer;" />';
      $output .= '<div id="' . $uc_type . '_address_select"></div>';
      return $output;

    case 'edit-theme':
      return '<div id="' . $uc_type . '-pane">' . drupal_render($form[$uc_type]) . '</div>';
      //$output = '<div id="' . $uc_type . '-pane"><div id="' . $uc_type . '_address_select"></div>' . drupal_render($form[$uc_type]) . '</div>';

    case 'edit-process':
      $order_values = $form_state['values'];
      $address = $order->uc_addresses[$type];
      $changes = array();

      foreach ($order_values as $key => $value) {
        // Check if this an address field
        if (strpos($key, $uc_type . '_') !== 0) {
          continue;
        }

        // Check if the value was changed
        $fieldname = substr($key, strlen($uc_type) + 1);
        try {
          if ($address->getField($fieldname) != $value) {
            $changes[$key] = $value;
          }
        }
        catch (UcAddressesException $e) {
          // Ignore any Ubercart Addresses exceptions.
        }
      }
      return $changes;
  }
}
