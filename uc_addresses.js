/**
 * Javascript functions for Ubercart Addresses
 */

/**
 * Keeps track of the copy address checkbox being checked or not
 * @var boolean
 */
var uc_addresses_copy_box_checked = false;

/**
 * (Re)triggers the click event for the copy address checkbox
 * if this checkbox is checked
 */
$(document).ready(uc_addresses_trigger_copy_address);

/**
 * Copy address values from one pane to another for checkout form
 */
function uc_addresses_copy_address_checkout(checked, source, target) {
  var address_pane_source = '#' + source + '-pane';
  var address_pane_target = '#' + target + '-pane';
  if (checked) {
    uc_addresses_copy_address(source, target, 'checkout_form');
    $(address_pane_target + ' div.address-pane-table').slideUp();
    uc_addresses_copy_box_checked = true;
  }
  else {
    $(address_pane_target + ' div.address-pane-table').slideDown();
    uc_addresses_copy_box_checked = false;
  }
}

/**
 * Copy address values from one pane to another for order form
 */
function uc_addresses_copy_address_order(source, target) {
  uc_addresses_copy_address(source, target, 'order_form');
}

/**
 * Copy address values from one pane to another
 *
 * @param string source
 *   The pane to get the address data from
 *   The pane to copy address data to
 * @param string target
 *   The pane to copy address data to
 * @param string context
 *   The context in which the address forms appear:
 *   - checkout_form
 *   - order_form
 *
 * @return void
 */
function uc_addresses_copy_address(source, target, context) {
  var address_pane_source = '#' + source + '-pane';
  var address_pane_target = '#' + target + '-pane';

  if (context == 'checkout_form') {
    // On the checkout form, the pane type is repeated 3 times.
    var source_source = source + '-' + source + '-' + source;
    var target_target = target + '-' + target + '-' + target;
    zone_field_source = '#edit-panes-' + source_source + '-zone';
    zone_field_target = '#edit-panes-' + target_target + '-zone';
  }
  if (context == 'order_form') {
    // On the order form, the pane type is repeated 2 times.
    var source_source = source + '-' + source;
    var target_target = target + '-' + target;
    zone_field_source = '#edit-' + source_source + '-zone';
    zone_field_target = '#edit-' + target_target + '-zone';
  }

  // Copy over the zone options manually.
  if ($(zone_field_target).html() != $(zone_field_source).html()) {
    $(zone_field_target).empty().append($(zone_field_source).children().clone());
    $(zone_field_target).attr('disabled', $(zone_field_source).attr('disabled'));
  }

  // For each input field
  $(address_pane_target + ' input, select, textarea', ':visible', document.body).each(
    function(i) {
      // Copy the values from the source pane to the target pane
      var source_field = this.id;
      source_field = source_field.replace(target_target, source_source);
      var target_field = this.id;
      if (target_field != source_field) {
        if (this.type == 'checkbox') {
          $('#' + target_field).attr('checked', $('#' + source_field).attr('checked'));
        }
        else {
          $('#' + target_field).val($('#' + source_field).val());
        }

        // Activate field event only on checkout form
        if (context == 'checkout_form') {
          $('#' + source_field).change(function () {uc_addresses_update_field(source_field, target_field); });
        }
      }
    }
  );
}

/**
 * Copy a value from one pane to another when the source field has been changed.
 *
 * @param string source_field
 *   ID of the field that has been changed
 * @param string target_field
 *   ID of the field to update
 *
 * @return void 
 */
function uc_addresses_update_field(source_field, target_field) {
  if (uc_addresses_copy_box_checked) {
    // Check if it is the zone field that changed.
    if (source_field.search('zone$') != -1) {
      if ($('#' + target_field).html() != $('#' + source_field).html()) {
        $('#' + target_field).empty().append($('#' + source_field).children().clone());
        $('#' + target_field).attr('disabled', $('#' + source_field).attr('disabled'));
      }
    }

    if ($('#' + source_field).attr('type') == 'checkbox') {
      $('#' + target_field).attr('checked', $('#' + source_field).attr('checked')).change();
    }
    else {
      source_value = $('#' + source_field).val();
      $('#' + target_field).val(source_value).change();
    }
  }
}

/**
 * (Re)triggers the click event for the copy address checkbox
 * if this checkbox is checked
 *
 * @return void
 */
function uc_addresses_trigger_copy_address() {
  // Check if any copy address checkboxes are checked
  // and (re)trigger the click event
  $('.uc_addresses_copy_address').each(
    function() {
      // Make sure the html element is a checkbox
      if (this.type == 'checkbox') {
        if (this.checked == true) {
          // The click event must be triggered twice because the 
          // first trigger causes to checkbox to be unchecked!
          $(this).trigger('click');
          $(this).trigger('click');
        }
      }
    }
  );
}

/**
 * Apply the selected address to the appropriate fields in the cart form.
 */
function uc_addresses_apply_address(type, address_str) {
  if (address_str == '0') {
    return;
  }
   
  var address_pane = '#' + type + '-pane';
  var order_field_id_prefix = 'edit-' + type + '-' + type + '-';
  var checkout_field_id_prefix = 'edit-panes-' + type + '-' + type + '-' + type + '-';

  eval('var address = ' + address_str + ';');

  $(address_pane + ' input, select, textarea', ':visible', document.body).each(
    function (i) {
      fieldname = this.id;
      fieldname = fieldname.replace(checkout_field_id_prefix, '');
      fieldname = fieldname.replace(order_field_id_prefix, '');

      if (fieldname != 'country' && fieldname != 'zone' && address[fieldname] != undefined) {
        if (this.type == 'checkbox') {
          $(this).attr('checked', (address[fieldname] == 1) ? true : false).trigger('change');
        }
        else {
          $(this).val(address[fieldname]).trigger('change');
        }
      }
    }
  );

  // Special treatment for country and zone fields
  // Checkout
  if ($('#' + checkout_field_id_prefix + 'country').val() != address.country) {
    try {
      $('#' + checkout_field_id_prefix + 'country').val(address.country);
      uc_update_zone_select(checkout_field_id_prefix + 'country', address.zone);
    }
    catch (err) { }
  }
  $('#' + checkout_field_id_prefix + 'zone').val(address.zone).trigger('change');
  // Order
  if ($('#' + order_field_id_prefix + 'country').val() != address.country) {
    try {
      $('#' + order_field_id_prefix + 'country').val(address.country);
      uc_update_zone_select(order_field_id_prefix + 'country', address.zone);
    }
    catch (err) { }
  }
  $('#' + order_field_id_prefix + 'zone').val(address.zone).trigger('change');
}
