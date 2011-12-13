<?php

/**
 * @file
 * Chosen administration pages.
 */

/**
 * General configuration form.
 */
function chosen_admin_settings() {
  $form = array();
  
  $form['chosen_minimum'] = array(
    '#type' => 'select',
    '#title' => t('Minimum number of options'),
    '#options' => array(
      0 => t('always apply'),
      5 =>  5,
      10 =>  10,
      15 =>  15,
      20 =>  20,
      25 =>  25,
    ),
    '#default_value' => variable_get('chosen_minimum', 20),
    '#description' => t('The mininum number of options to apply Chosen. Example : choosing 10 will only apply Chosen if the number of options is greater or equal to 10.')
  );
  
  $form['chosen_minimum_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Minimum width of widget'),
    '#field_suffix' => 'px',
    '#required' => TRUE,
    '#size' => 3,
    '#default_value' => variable_get('chosen_minimum_width', 200),
    '#description' => t('The minimum width of the Chosen widget.'),
  );
  
  $form['chosen_jquery_selector'] = array(
    '#type' => 'textarea',
    '#title' => t('Apply Chosen to the following elements'),
    '#description' => t('A jQuery selector to find elements to apply Chosen to, such as <code>.chosen-select</code>. Defaults to <code>select</code> to apply Chosen to all <code>&lt;select&gt;</code> elements.'),
    '#default_value' => variable_get('chosen_jquery_selector', 'select:visible'),
  );
  
  return system_settings_form($form);
}
