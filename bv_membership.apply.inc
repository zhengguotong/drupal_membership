<?php

function bv_membership_member_apply_page($form, &$form_state){
    global $base_url;

    $form = array();

    $form['#attached']['js'] = array(
        drupal_get_path('module', 'bv_membership') . '/js/jquery.cityselect.js',
    );

    $form['member'] = array(
        '#type' => 'fieldset',
        '#title' => t('申请会员'),
    );

    $form['member']['bank_name'] = array(
        '#type' => 'textfield',
        '#title' => t('银行名称'),
        '#size' => 60,
        '#maxlength' => 128,
    );

    $form['member']['bank_account'] = array(
        '#type' => 'textfield',
        '#title' => t('银行账号'),
         '#size' => 60,
        '#maxlength' => 128,
    );


    $form['member']['parent_phone'] = array(
        '#type' => 'textfield',
        '#title' => t('推荐人手机号码'),
        '#description' => t('若无推荐，请输入你的地址，我们会根据你的地址选择默认推荐人。'),
        '#size' => 60,
        '#maxlength' => 128,
        '#element_validate' => array('_bv_membership_china_phone_validation'),
    );

    $form['member']['city']= array( '#markup' => '<div class="city-select"><label class="control-label">地址</label><select class="form-control prov"></select>'
        . '<select class="form-control city" disabled="disabled"></select><select class="form-control dist" disabled="disabled"></select></div>' );

    $form['member']['provh'] = array('#type' => 'hidden',
                                    '#default_value' => '' ,
                                    '#attributes' => array('class' => array('provh')));

    $form['member']['cityh'] = array('#type' => 'hidden',
                                    '#default_value' => '',
                                    '#attributes' => array('class' => array('cityh')));

    $form['member']['disth'] = array('#type' => 'hidden',
                                    '#default_value' => '',
                                    '#attributes' => array('class' => array('disth')));

    $form['member']['baseurlh'] = array('#type' => 'hidden',
                                        '#value' =>$base_url . '/' . drupal_get_path('module', 'bv_membership') . '/js/city.min.js',
                                        '#attributes' => array('class' => array('baseurlh')));

    $form['member']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('submit')
    );

    return $form;
}

/**
 * Make sure parent phone number is in correct format
 * @param $element
 * @param $form_state
 */
function _bv_membership_china_phone_validation($element, &$form_state){
    ddl($form_state);
    if (!empty($element['#value'])&& !_bv_membership_valid_cn_phone_number($element['#value'])){
      form_error($element,  t('请输入正确的推荐人手机号码.'));
  }
}


/**
 * Verifies that $phonenumber is a valid Chinese phone number
 *
 * @param string $phonenumber
 * @return boolean Returns boolean FALSE if the phone number is not valid.
 */
function _bv_membership_valid_cn_phone_number($phonenumber) {

    $phonenumber = trim($phonenumber);

    // define regular expression
    $regex = '/^(\+86|86)?( |-)?([0-9]{11}|([0-9]{3,4}(\-|\.| )[0-9]{3,8})|[0-9]{2}( |\-)[0-9]{4}[ ][0-9]{4}|[0-9]{2}\.[0-9]{2}\.[0-9]{2}\.[0-9]{2}\.[0-9]{2})$/';

    // return true if valid, false otherwise
    return (bool) preg_match($regex, $phonenumber);
}