<?php

$do = new apps();
$do->process();

class apps{

    function __construct() {

        date_default_timezone_set('America/Chicago');

        error_reporting(E_ALL);
        ini_set ('display_errors', false);

    }

    function process() {

        // parse the querystring
        // if username and password are set, process, otherwise generate an error
        // query the database to see which publications the subscrber has access to
        // return the list of publications, or an appropriate error message, e.g., subscription expired
        // $iv = mcrypt_create_iv(mcrypt_get_iv_size(MCRYPT_RIJNDAEL_256, MCRYPT_MODE_ECB), MCRYPT_RAND);
        (isset($_SERVER['QUERY_STRING'])) ? parse_str($_SERVER['QUERY_STRING'], $querystring) : $querystring = array();


        $rsspath = '';
        if ((isset($_POST['usr'])) && (isset($_POST['pwd']))) {


            (isset($_POST['device'])) ? $device = $_POST['device'] : $device = '';
            $params['0'] = array(
                $_POST['usr']
            , $_POST['pwd']
            , $device
            );

            if ((isset($querystring['do'])) && ($querystring['do'] == "0")) echo $this->getEntitlements($params);
            else echo $this->respondToSender($this->getEntitlements($params));

        }

    }

    function encrypt_decrypt($action, $string) {
        $output = false;

        $encrypt_method = "AES-256-CBC";
        $secret_key = 'This is my secret key';
        $secret_iv = 'This is my secret iv';

        // hash
        $key = hash('sha256', $secret_key);

        // iv - encrypt method AES-256-CBC expects 16 bytes - else you will get a warning
        $iv = substr(hash('sha256', $secret_iv), 0, 16);

        if( $action == 'encrypt' ) {
            $output = openssl_encrypt($string, $encrypt_method, $key, 0, $iv);
            $output = base64_encode($output);
        }
        else if( $action == 'decrypt' ){
            $output = openssl_decrypt(base64_decode($string), $encrypt_method, $key, 0, $iv);
        }

        return $output;
    }





    function getEntitlements($params) {

        //$query = $this->getEntitlementsQuery2();
        //$entitlements = $this->queryMSSQL('Etype', $query, $params);
        //echo "entitlements: ", count($entitlements), " results\n"; print_r($entitlements); echo "\n";

        $username= $params['0']['0'];
        $Password= $params['0']['1'];
        $device= $params['0']['2'];
        // this result means the username/password did not return a record
        $param12=array('UserName'=>"$username");
        $client12= new soapclient('http://etypeservices.com/service_GetPublicationIDByUserName.asmx?WSDL');
        $response12=$client12->GetPublicationID($param12);

        if($response12->GetPublicationIDResult== -9)
        {
            $return = $this->buildOutput("KO", "User or password invalid. Please try again");
        }  else
        {

            if($response12->GetPublicationIDResult== 606 || $response12->GetPublicationIDResult== 611 || $response12->GetPublicationIDResult== 613  || $response12->GetPublicationIDResult== 614 || $response12->GetPublicationIDResult== 615 || $response12->GetPublicationIDResult== 617 || $response12->GetPublicationIDResult== 618	|| $response12->GetPublicationIDResult== 619 || $response12->GetPublicationIDResult== 620 || $response12->GetPublicationIDResult== 621 || $response12->GetPublicationIDResult== 2417 || $response12->GetPublicationIDResult== 2533 || $response12->GetPublicationIDResult== 3518 || $response12->GetPublicationIDResult== 3519 || $response12->GetPublicationIDResult== 3520 || $response12->GetPublicationIDResult== 3521 || $response12->GetPublicationIDResult== 3899 || $response12->GetPublicationIDResult== 4279)
            {

                $param=array('Usr' =>"$username",'Pwd' =>"$Password",'Device' =>"$device");
                $client= new soapclient('http://www.etypeservices.com/Service_OKJailbirdsSubscriberLogin.asmx?wsdl');

                $response=$client->GetDetailsByUserName($param);
                //   echo "<pre>";
                //  print_r($param);
                //  echo "</pre>";
                if ($response->GetDetailsByUserNameResult->UserDetails->UserIsValid== -5) {
                    $return = $this->buildOutput("KO", "User or password invalid. Please try again");
                }

                // username/password did return records from the db
                elseif ($response->GetDetailsByUserNameResult->UserDetails->UserIsValid == -1) {

                    $expired = array(); // expired subscriptions
                    $canView = array(); // active subscriptions
                    //$pubview =array();
                    $outputStarter = "<key>output</key><array>";
                    $outputBody = '';
                    $outputEnder = "</array>";
                    $pubview= $response12->GetPublicationIDResult;

                    $pre="Evangeline";
                    $canView[strtolower($pre)] = true;

                    $return = $this->buildOutput("KO", "Your subscription to Evangeline Today expired . Please contact the Helpdesk to renew your subscription", $canView,$pubview,$pre);

                }
                else {
                    $expired = array(); // expired subscriptions
                    $canView = array(); // active subscriptions
                    //$pubview =array();
                    $outputStarter = "<key>output</key><array>";
                    $outputBody = '';
                    $outputEnder = "</array>";
                    $pubview= $response12->GetPublicationIDResult;
                    $userdetail="".trim($username)."--".trim($Password)."";

                    $shortpre1=array("606"=>'okj',"611"=>'pay',"613"=>'com',"614"=>'can',"615"=>'gar',"617"=>'ste',"618"=>'lak',"619"=>'pot',"620"=>'kay',"621"=>'lov',"2417"=>'se',"2533"=>'sw',"3518"=>'casm',"3519"=>'rncmd',"3520"=>'llpmh',"3521"=>'mcba',"3899"=>'galb',"4279"=>'cclm');


                    $param1=array('Username' =>"$username",'PublicationID' =>"$pubview");

                    $client1= new soapclient('http://etypeservices.com/GetSubscriberExpiryDate.asmx?wsdl');

                    $response1=$client1->Get_SubscriberExpiryDate($param1);
                    $dateexp=$response1->Get_SubscriberExpiryDateResult;

                    $d=$response->GetDetailsByUserNameResult->UserDetails->ExpiryDate;
                    $expdate=date("jS F Y", strtotime($d));
                    $msg="Your subscription will expire on " .$expdate;


                    // setcookie("etypes[userdetail]",$encrypted_txt, time()+3600*24);
                    $pre=$response->GetDetailsByUserNameResult->UserDetails->PublicationID;
                    // echo $pre;
                    // die();
                    $canView=rtrim($pre, ",");
                    $shortpreid= explode(",", $canView);
                    $shortpre=array();

                    foreach($shortpre1 as $x => $shortpre11)
                    {
                        if (in_array($x, $shortpreid))
                        {
                            $shortpre[]=$shortpre11;
                        }
                    }

                    //  echo "<pre>";
                    //	 print_r($shortpre);
                    //	 echo "</pre>";
                    //	echo $canView;
                    //	 die();
                    //$canView[strtolower($pre)] = true;

                    $return = $this->buildOutput("OK",$msg,$shortpre,$canView,$pubview,$pre,$userdetail,$dateexp);
                }
            }
            else{
                $return = $this->buildOutput("KO", "User or password invalid. Please try again");
            }





        }
        return $return;

    }


    function respondToSender($response) {

        header('Content-Type: application/xml');
        print $response;

    }


    function buildOutput($code, $msg, $shortpre, $outputs='',$pubview,$pre,$userdetail,$dateexp) {


        $pubs = '';
        $out = '';
        $out .= "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>";
        $out .= "<!DOCTYPE plist PUBLIC \"-//Apple Inc//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">";
        $out .= "<plist version=\"1.0\">";
        $out .= "<dict>";
        $out .= "<key>code</key>";
        $out .= "<string>" . trim($code) . "</string>";
        $out .= "<key>msg</key>";
        $out .= "<string>";
        $out .= trim($msg);
        $out .= "</string>";

        // take note of the pubs the user can see

        if (!empty($outputs)) {
            $aout=explode(",",$outputs);
            $out .= "<key>output</key><array>";
            foreach($shortpre as $shortprevalue)
            {
                $out .= "<string>" . $shortprevalue . "</string>";
            }
            foreach ($aout as $outputKey) {
                $out .= "<string>" . strtolower($outputKey) . "</string>";
                // also keep track for the cookie
                $pubs .= $outputKey . ", ";
            }
            // $out .= "<string>" . trim($shortpre) . "</string>";
            $out .= "</array>";

        }


        $out .= "</dict>";
        $out .= "</plist>";
        $publ=$pubview;

        $perms="".trim($code)."_".trim($publ)."##".trim($userdetail)."";

        $encrypted_txt = $this->encrypt_decrypt('encrypt', $perms);

        setcookie("etype[perms]",$encrypted_txt,strtotime($dateexp));

        return $out;

    }

}
