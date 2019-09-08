# $previousStatus = git branch
# $previousStatus = $previousStatus.split('\n')
# $previousBranch = ""
# $targetBranch = "Daily"
$targetRemote = "repo"
$goodStatus = "nothing to commit, working tree clean"
$currentStatus = git status
# For ($i = 0; $i -lt $previousStatus.Count; $i++) {
#     If ($($($previousStatus[$i])[0]) -eq '*') {
#         $previousBranch = $($($previousStatus[$i]).split(' ')[1])
#     }
# }
# # echo $previousBranch
# if ($previousBranch -ne $targetBranch) {
#     git checkout $targetBranch
# }
if ($currentStatus.Contains($goodStatus)) {
    Write-Output $goodStatus
}
else {
    $date = Get-Date
    git add *
    if ($args.Count -eq 0) {
        git commit -a -m "$date" 
    }
    else {
        git commit -a -m "$($args[0])"
    }
    git push $targetRemote    
}
# if ($previousBranch -ne $targetBranch) {
#     git checkout $previousBranch
# }