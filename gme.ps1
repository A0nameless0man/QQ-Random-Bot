$previousStatus = git branch
$previousStatus = $previousStatus.split('\n')
$previousBranch = ""
$targetRemote = "repo"
$targetBranch = "master"
For ($i = 0; $i -lt $previousStatus.Count; $i++) {
    If ($($($previousStatus[$i])[0]) -eq '*') {
        $previousBranch = $($($previousStatus[$i]).split(' ')[1])
    }
}
# echo $previousBranch
if ($args.Count -eq 0) {
    ./gcm.ps1
}else {
    ./gcm.ps1 "$($args[0])"
}

if ($previousBranch -ne $targetBranch) {
    git checkout $targetBranch
}
if ($args.Count -eq 0) {
    git merge $previousBranch
}else {
    git merge $previousBranch -m "$($args[0])"
}
git push $targetRemote
if ($previousBranch -ne $targetBranch) {
    git checkout $previousBranch
}