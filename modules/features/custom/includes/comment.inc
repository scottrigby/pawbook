<?php

/**
 * @file
 * Functions for Comment module.
 */

/**
 * Implements hook_form_FORM_ID_alter().
 */
function custom_form_comment_node_book_form_alter(&$form, &$form_state, &$form_id) {
  $form['comment_body']['#after_build'][] = 'custom_customize_comment_form';
}

/**
 * After build callback for book comment form.
 */
function custom_customize_comment_form($element) {
  // Set comments to plain_text only & hide options.
  $element[$element['#language']][0]['format']['format']['#access'] = FALSE;
  $element[$element['#language']][0]['format']['format']['#value'] = 'plain_text';
  // Hide guidelines for all but plain_text.
  foreach (element_children($element[$element['#language']][0]['format']['guidelines']) as $format) {
    if ($format != 'plain_text') {
      $element[$element['#language']][0]['format']['guidelines'][$format]['#access'] = FALSE;
    }
  }
  return $element;
}
