<?php
// Define a class for a person
class Person {
  private $name;
  private $age;
  private $gender;

  // Constructor method
  public function __construct($name, $age, $gender) {
    $this->name = $name;
    $this->age = $age;
    $this->gender = $gender;
  }

  // Getter methods
  public function getName() {
    return $this->name;
  }

  public function getAge() {
    return $this->age;
  }

  public function getGender() {
    return $this->gender;
  }

  // Setter methods
  public function setName($name) {
    $this->name = $name;
  }

  public function setAge($age) {
    $this->age = $age;
  }

  public function setGender($gender) {
    $this->gender = $gender;
  }

  // Method to print person details
  public function printDetails() {
    echo "Name: " . $this->name . "<br>";
    echo "Age: " . $this->age . "<br>";
    echo "Gender: " . $this->gender . "<br>";
  }
}

// Create some person objects
$person1 = new Person("John Doe", 30, "Male");
$person2 = new Person("Jane Doe", 25, "Female");

// Print details of person1
echo "<h2>Person 1</h2>";
$person1->printDetails();

// Print details of person2
echo "<h2>Person 2</h2>";
$person2->printDetails();
$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();$person2->printDetails();
?>
