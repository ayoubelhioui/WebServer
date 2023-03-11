<?php
if (isset($_POST['submit'])) {
    $video_name = $_FILES['video_file']['name'];
    $video_size = $_FILES['video_file']['size'];
    $video_tmp = $_FILES['video_file']['tmp_name'];
    $video_type = $_FILES['video_file']['type'];
    $video_ext = strtolower(pathinfo($video_name, PATHINFO_EXTENSION));

    // Set the directory where the uploaded video will be stored
    $upload_dir = 'uploads/videos/';

    // Create the directory if it doesn't already exist
    if (!file_exists($upload_dir)) {
        mkdir($upload_dir, 0777, true);
    }

    // Define the allowed file extensions
    $allowed_extensions = array('mp4', 'mov', 'avi', 'wmv');

    // Check if the uploaded file has an allowed extension
    if (in_array($video_ext, $allowed_extensions)) {
        // Move the uploaded file to the specified directory
        $destination = $upload_dir . $video_name;
        move_uploaded_file($video_tmp, $destination);
        echo "tmp : $video_tmp<br/>";

        echo 'Video uploaded successfully.';
    } else {
        echo 'Error: Invalid file type. Only MP4, MOV, AVI, and WMV files are allowed.';
    }
}
?>