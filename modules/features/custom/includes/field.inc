<?php

/**
 * @file
 * Functions for Field module.
 */

/**
 * Implements hook_preprocess_HOOK().
 */
function custom_preprocess_field(&$variables) {
  // Add new field element class to textfields, based on field format. This lets
  // us target fields with specific formats for theming.
  $field_type = $variables['element']['#field_type'];
  if ($field_type == 'text_with_summary') {
    $items = $variables['element']['#items'];
    if (!empty($items)) {
      foreach ($items as $item) {
        $format = $item['format'];
        $variables['classes_array'][] = 'field-format-' . drupal_html_class($format);
      }
    }
  }
}
